# Dump data_dict.inl from dict.txt.

def dump_dict_cpp(fi, fo):
    fo.write('// NOTE: This file was automatically generated.\n\n')
    fo.write('const DataEntry g_data_dict[] = {\n')

    count = 0

    for line in fi:
        line = line.strip(' \t\n')
        if not line:
            continue

        parts = line.split('; ')
        
        if len(parts) != 4:
            print('Invalid line:', line)
            break

        fo.write('  { ')
        fo.write('{0}, VR::{1}, "{2}", "{3}"'.format(parts[0], parts[1], parts[2], parts[3]))
        fo.write(' },\n')

        count += 1

    fo.write('};\n\n')

    fo.write('const std::size_t DATA_DICT_SIZE = {};\n'.format(count))


with open("dict.txt", 'r') as fi, open('data_dict.inl', 'w') as fo:
    dump_dict_cpp(fi, fo)
