import parser

with open("example.hy") as f:
    text = f.read()
def pprint(x, level=0):
    l = level * "|"
    try:
        x[0]
        if type(x) == str:
            raise TypeError
        for c in x:
            pprint(c, level+1)
    except:
        print(l + str(x))

pprint(parser.parse(parser.lex(text)))