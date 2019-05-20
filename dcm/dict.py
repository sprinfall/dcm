# Generate dict.inl from dict.txt.

def dump_dict_cpp(fi, fo):
    fo.write('// NOTE: This file was automatically generated.\n\n')
    fo.write('static const Entry g_dict[] = {\n')

    for line in fi:
        line = line.strip(' \t\n')
        if not line:
            continue

        parts = line.split('; ')
        
        if len(parts) != 4:
            print('Invalid line:', line)
            break

        fo.write('  { ')
        fo.write('{0}, VR::{1}, "{2}"'.format(parts[0], parts[1], parts[2]))
        fo.write(' },\n')

    fo.write('};\n\n')


with open("dict.txt", 'r') as fi, open('dict.inl', 'w') as fo:
    dump_dict_cpp(fi, fo)
