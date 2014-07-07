#! /usr/bin/python

if __name__ == '__main__':
	infile = open('recorder-mpi-init-finalize-compress.c')
	outfile = open('func_list_from_decl', 'w')

	for line in infile:
		if line.startswith('RECORDER_FORWARD_DECL'):
			striped = line[len('RECORDER_FORWARD_DECL')+2:]
			pos1 = striped.find(',')
			pos2 = striped.find(',', pos1+1)
			func = 'int ' + striped[:pos1] + striped[pos2+2:-3] + ';'
			outfile.write(func + '\n')
