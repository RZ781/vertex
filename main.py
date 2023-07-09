import os
files = ["main", "lexer", "parser", "data", "parselib", "ir"]
includes = ["lexer", "parser", "debug", "parselib", "ir"]

code = ""
for file in files:
    with open(f"c/{file}.c") as f:
        code += f.read()
for file in includes:
    with open(f"include/{file}.h") as f:
        code += f.read()
hash1 = str(hash(code))
with open("hash") as f:
    hash2 = f.read()
    
if hash1 != hash2:
    print("building...")
    with open("hash", "w") as f:
        f.write(hash1)
    os.system("./build.sh")
os.system("./vertex")