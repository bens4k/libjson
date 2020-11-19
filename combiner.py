import os

start_includes = '/*start includes*/'
end_includes = '/*end includes*/'

start_typedefs = '/*start of typedef*/'
end_typedefs = '/*end of typedef*/'

start_structs = '/*start of struct defs*/'
end_structs = '/*end of struct defs*/'

start_func_protos = '/*start of func protos*/'
end_func_protos = '/*end of func protos*/'

start_func_defs = '/*start of func defs*/'
end_func_defs = '/*end of func defs*/'

headers_path = os.path.join('seperated', 'include')
sources_path = os.path.join('seperated', 'src')
outfile = os.path.join('combined', 'json.c')

headers = list(filter(lambda x : x.endswith('.h'), os.listdir(headers_path)))
sources = list(filter(lambda x : x.endswith('.c'), os.listdir(sources_path)))

output = '#include "json.h"\n'

# adding includes 
for header in headers:
    with open(os.path.join(headers_path, header)) as f:
        header_content = f.read()
        firstsplit = header_content.split(start_includes)
        if len(firstsplit) > 1:
            output += firstsplit[1].split(end_includes)[0]

# typedefs
for header in headers:
    with open(os.path.join(headers_path, header)) as f:
        header_content = f.read()
        firstsplit = header_content.split(start_typedefs)
        if len(firstsplit) > 1:
            output += firstsplit[1].split(end_typedefs)[0]

# sort out dependancies
headers.insert(0, headers.pop(headers.index('stack.h')))

# struct defs in headers 
for header in headers:
    with open(os.path.join(headers_path, header)) as f:
        header_content = f.read()
        firstsplit = header_content.split(start_structs)
        if len(firstsplit) > 1:
            output += firstsplit[1].split(end_structs)[0]

# struct defs in sources 
for source in sources:
    with open(os.path.join(sources_path, source)) as f:
        source_content = f.read()
        firstsplit = source_content.split(start_structs)
        if len(firstsplit) > 1:
            output += firstsplit[1].split(end_structs)[0]

# function prototypes in headers
for header in headers:
    with open(os.path.join(headers_path, header)) as f:
        header_content = f.read()
        firstsplit = header_content.split(start_func_protos)
        if len(firstsplit) > 1:
            output += firstsplit[1].split(end_func_protos)[0]

# function defs in sources
for source in sources:
    with open(os.path.join(sources_path, source)) as f:
        source_content = f.read()
        firstsplit = source_content.split(start_func_defs)
        if len(firstsplit) > 1:
            output += firstsplit[1].split(end_func_defs)[0]

with open(outfile, 'w') as out:
    out.write(output)

# create the header file without the includes 
outheader = ""
with open(os.path.join('seperated', 'include', 'json.h'), 'r') as f:
    contents = f.read()
    lines = contents.split('\n')
    lines = list(filter(lambda x: not x.startswith('#include'), lines))
    outheader = '\n'.join(map(str, lines))

with open('combined/json.h', 'w') as fout:
    fout.write(outheader)

