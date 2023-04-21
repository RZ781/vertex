import re

regexp = "|".join([
    "//[^\\n]*", # single line comments
    "[\\\\`~!@#$%^&*\\(\\)\\-=+\\[\\]{}\\|;':,./<>?]", # symbols
    "\".*\"", # strings
    "[a-zA-Z_][a-zA-Z0-9_]*", # names
    "-?[0-9]+", # integers
    "\\s+" # whitespace
])
lkws = ["raise", "return", "yield", "await", "break", "continue"]
kws = ["func", "import", "if", "else", "do", "while", "for", "in", "try", "catch", "finally", "class", "let"]

def tag(tokens): # give a tag or type to every token
    out = []
    pos, line = 0, 0
    for token in tokens:
        if token.isspace(): # whitespace
            pass
        elif token[0] == "/" and len(token) > 1: # comment
            pass
        elif token.isdecimal(): # integer
            out.append(("int", token, pos, line))
        elif token[0] == '"' and token[-1] == '"': # string
            out.append(("str", token[1:-1], pos, line))
        elif token in lkws:
            out.append(("lkw", token, pos, line))
        elif token in kws:
            out.append((token, token, pos, line))
        elif token[0].isalpha(): # identifiers
            out.append(("id", token, pos, line))
        else: # symbols
            out.append((token, token, pos, line))
        for c in token:
            pos += 1
            if c == '\n':
                pos = 0
                line += 1
    return out

def lex(text):
    tokens = re.findall(regexp, text)
    lexed = "".join(tokens) # original reconstructed from tokens
    if lexed != text: # not all of the text was converted to tokens
        raise SyntaxError("unclosed string")
    return tag(tokens)

def parse(x):
    global tokens, i
    tokens = x
    i = 0
    stmts = []
    while peek() != "eof":
        stmts.append(stmt())
    return stmts
def peek(n=0):
    if i + n >= len(tokens):
        return "eof"
    return tokens[i+n][0]
def next():
    global i
    i += 1
    if i + 1 >= len(tokens):
        return ("eof", "", -1, -1)
    return tokens[i-1]
def eat(x):
    if peek() != x:
        t, _, pos, line = next()
        raise SyntaxError(f"line {line} pos {pos}: expected {x}, got {t}")
    return next()
def stmt():
    e = expr()
    if peek(-1) != "}":
        eat(";")
    return e
def l_expr():
    global i
    token = next()
    match token[0]:
        case "{":
            prev_i = i
            try: # block
                stmts = []
                while peek() != "}":
                    stmts.append(stmt())
                eat("}")
                return ("block", stmts)
            except: # dictionary
                i = prev_i
                entries = []
                while peek() != "}":
                    a = expr()
                    eat(":")
                    b = expr(1)
                    entries.append((a, b))
                    if peek() == "}":
                        break
                    eat(",")
                eat("}")
                return ("dict", entries)
        case "(":
            e = expr()
            eat(")")
            return e
        case "[":
            l = []
            while peek() != "]":
                l.append(expr(1))
                if peek() == "]":
                    break
                eat(",")
            eat("]")
            return ("list", l)
        case "lkw":
            arg = expr()
            return ("lstmt", token[1], arg)
        case "if":
            cond = expr()
            body = expr()
            if peek() == "else":
                eat("else")
                else_b = expr()
                return ("ifelse", cond, body, else_b)
            return ("if", cond, body)
        case "for":
            eat("(")
            a = expr()
            if peek() == ";":
                eat(";")
                b = expr()
                eat(";")
                c = expr()
                eat(")")
                body = expr()
                return ("for", a, b, c, body)
            else:
                eat("in")
                b = expr()
                eat(")")
                body = expr()
                return ("forin", a, b, body)
        case "while":
            cond = expr()
            body = expr()
            return ("while", cond, body)
        case "do":
            body = expr()
            eat("while")
            cond = expr()
            return ("dowhile", cond, body)
        case "try":
            body = expr()
            catch = []
            fin = None
            while peek() == "catch":
                eat("catch")
                catch.append((expr(), expr()))
            if peek() == "finally":
                eat("finally")
                fin = expr()
            return ("try", body, catch, fin)
        case "func":
            name = ""
            if peek() == "id":
                name = eat("id")
            eat("(")
            params = []
            while peek() != ")":
                params.append(eat("id"))
                if peek() == ")":
                    break
                eat(",")
            eat(")")
            body = expr()
            return ("func", params, body)
        case "class":
            name = ""
            if peek() == "id":
                name = eat("id")
            body = expr()
            return ("class", name, body)
        case "+":
            e = expr()
            return ("+", e)
        case "-":
            e = expr()
            return ("-", e)
        case "!":
            e = expr()
            return ("not", e)
        case "import":
            names = [eat("id")]
            while peek() == ".":
                eat(".")
                names.append(eat("id"))
            return ("import", names)
        case "let":
            name = eat("id")
            if peek() == "=":
                eat("=")
                return ("letval", name, expr())
            return ("let", name)
        case "id":
            return ("id", token)
        case "int":
            return ("int", token)
        case "str":
            return ("str", token)
        case _:
            _, _, pos, line = next()
            raise SyntaxError(f"line {line} pos {pos}: unexpected {token[0]}")
def nop_expr():
    e = l_expr()
    while True:
        match peek():
            case ".":
                eat(".")
                e = ("attr", e, eat("id"))
            case "(":
                eat("(")
                e = ("call", e, expr())
                eat(")")
            case _:
                return e
def expr(x=0):
    e = [nop_expr()]
    while peek() in "+-*/=,%^&|<>":
        if x and peek() == ",":
            break
        token = next()[0]
        if peek() == "=":
            eat("=")
            e.append(("sym", token))
        e.append(nop_expr())
    return ("expr", e)