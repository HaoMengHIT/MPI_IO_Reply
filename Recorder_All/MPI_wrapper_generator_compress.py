#! /usr/bin/python
# usage ./MPI_wrapper_generator_compress.py init-compress.c sample_output.c func_info

import re
import sys
# TODO: fh's value is not consistant

def generate_head(code_file, out_file):
	data = '/* \n * This file is generated automaticaly by a wrapper generator,\n'
	data += ' * Do not edit it manually.\n'
	data += ' * define neccessary variables below\n'
	data += ' */\n\n'

	data += '//#define _GNU_SOURCE\n'
	data += '#include <mpi.h>\n'
	data += '#include "simple_compress.h"\n'
	# sc should be initialized in input header
	data += 'SimpleCompress *sc;\n\n'

	data += 'struct timespec tm1, tm2, tm3, tm4, compute_time, mpi_time, this_func_time;\n'
	data += 'struct timespec compute_time_all, mpi_time_all, recorder_time_all;\n'
	data += 'struct timespec write_time_all, read_time_all;\n'
	data += 'int ret, result_len, array_size, s_offset;\n'
	data += 'char comm_name[100], newcomm_name[100], etype_name[100], filetype_name[100], datatype_name[100], oldtype_name[100], newtype_name[100], temp_datatype_name[100];\n'
	data += '#define AS 300000\n'
	data += 'char array_of_requests_arraystore[AS], array_of_gsizes_arraystore[AS], array_of_distribs_arraystore[AS], array_of_dargs_arraystore[AS], array_of_psizes_arraystore[AS], array_of_sizes_arraystore[AS], array_of_subsizes_arraystore[AS], array_of_starts_arraystore[AS], array_of_blocklengths_arraystore[AS], array_of_displacements_arraystore[AS], array_of_types_arraystore[AS];\n'
	# add buffer
	data += '#define BUFFER_SIZE 4096\n'
	data += 'const int threshold = BUFFER_SIZE * 0.9;\n'
	data += 'int bytes, written_bytes;\n'
	data += 'char rec_buffer[4096];\n\n'
	data += '#include <mpi.h>\n'
	data += 'MPI_Datatype* temp_datatype;\n'


# copy initialzation from recorder
	try:
		all_the_text = code_file.read()
	finally:
		code_file.close()

	data += all_the_text
	out_file.write(data)

	return


def generate_one_function(para_list, out_file, func_file):
	if len(para_list) < 3:
		return 1

	func_info = ''	# info for replay
# generate function head
	head = ''
	head += para_list[1] + ' '	# return type
	func_name = para_list[0].lstrip('P') + '('
	func = para_list[0].lstrip('P')
	func_info += func + ' '
	head += func_name	# function name

	for index, para in enumerate(para_list):
		if index > 1:
			head += para + ','
	
	head = head[:-1]
	head += ')\n'

# generate function body
	paras = para_list[2:]
	
	def datatype_code(datatype, pointer=False):
		if pointer == False:
			datatype_code = '''PMPI_Type_get_name(%(pointer)s%(data)s, %(data)s_name, &result_len);\n
		if (result_len == 0) {
			sprintf(%(data)s_name, "%%lu", %(pointer)s%(data)s);
		}
		'''
		else:
			datatype_code = '''sprintf(%(data)s_name, "%%lu", %(pointer)s%(data)s);\n'''
		if pointer == False:
			result = datatype_code % {'pointer' : '', 'data' : datatype}
		else:
			result = datatype_code % {'pointer' : '*', 'data' : datatype}
		return result

	def comm_code(comm):
		com_code = '''PMPI_Comm_get_name(%(pointer)s%(data)s, %(data)s_name, &result_len);\n
		if (result_len == 0) {
			sprintf(%(data)s_name, "%%lu", %(pointer)s%(data)s);
		}
		'''
		result = com_code % {'pointer' : '', 'data' : comm}
		return result

	get_name = {'comm': comm_code('comm'), 
				'newcomm' : comm_code('newcomm'),
				'datatype': datatype_code('datatype'),
				'etype': datatype_code('etype'),
				'filetype': datatype_code('filetype'),
				'oldtype' : datatype_code('oldtype'),
				'newtype': 'sprintf(newtype_name, "%lu", *newtype);\n'}

	para_format = {
			'MPI_File' : '%lu',
			'MPI_Offset' : '%lld',
			'MPI_Datatype' : '%s',
			'MPI_Comm' : '%s',
			'MPI_Status' : '%lu',
			'MPI_Info' : '%lu',
			'MPI_Op' : '%lu',
			'MPI_Request' : '%lu',
			'MPI_Aint' : '%ld',	# I assume MPI_Aint is the equivalent of ptrdiff_t, which is long int
			'int' : '%d',
			'char' : '%s',
			}
				
	call_para = '('
	extra_data = ''
	after_call = ''
	# TODO: the tm1 tm2 written to th file is really compute_time and mpi_time
	print_format_1 = '"tm1=%ld.%.9ld tm2=%ld.%.9ld func=' + para_list[0].lstrip('P') + ' '
	print_format_2 = 'compute_time.tv_sec, compute_time.tv_nsec, mpi_time.tv_sec, mpi_time.tv_nsec'
	call_delim = ', '
	delim = ' '
	para_delim = ', '
	get_array_size = ''
	get_array_content = ''

	body = '{\n'
	# print func
	if func == 'MPI_File_close':
		body += '\tMPI_File real_fh = *fh;\n'
	elif func == 'MPI_Wait':
		body += '\tMPI_Request real_request = *request;\n'
		

	for para in paras:
		type_of_para =  str(para.split()[0])
		true_para = str(para.split()[1])
		pointer = true_para.startswith('*')
		array = true_para.endswith('[]')
		call_para += true_para.lstrip('*').rstrip('[]')
		call_para += call_delim
		true_para = true_para.rstrip('[]')

		func_info += true_para.lstrip('*') + ' '
		
		if true_para.lstrip('*').startswith('array'):
			array = True;
			if pointer == True:
				pointer = False

		if type_of_para == 'MPI_Status':
			array = False

		
		extra = get_name.get(true_para.lstrip('*'), None)
		if extra is not None and array == False:
			# for MPI_Datatype* newtype
			if pointer == True:
				if type_of_para == 'MPI_Datatype' or type_of_para == 'MPI_Comm':
					true_true_para = true_para.lstrip('*')
					after_call += 'sprintf(%s_name, "%%lu", *' % true_true_para + true_para.lstrip('*') + ');\n'
			else:
				extra_data += extra
			
			extra = None
			
			print_format_1 += true_para.lstrip('*') + '=' + para_format.get(type_of_para, 'Error') + delim
			print_format_2 += para_delim + true_para.lstrip('*') + '_name'
		elif pointer == True and array == False:
			# char*
			if type_of_para == 'char':
				print_format_1 += true_para.lstrip('*') + '=' + para_format.get(type_of_para, 'Error') + delim
				print_format_2 += para_delim + true_para.lstrip('*')
			# for void *buf, int *comm, int *size, etc.
			elif type_of_para == 'void' or type_of_para == 'int':
				print_format_1 += true_para.lstrip('*') + '=' + true_para.lstrip("*") + delim
			else:
				#print_format_1 += true_para.lstrip('*') + '=' + para_format.get(type_of_para, 'Error') + delim
				# get address
				print_format_1 += true_para.lstrip('*') + '=' + "%lu" + delim
				if type_of_para == 'MPI_File' or type_of_para == "MPI_Request": # only use address
					#print_format_2 += ', ' + true_para
					if func == 'MPI_File_close':
						print_format_2 += para_delim + 'real_fh'
					elif func == 'MPI_Wait':
						print_format_2 += para_delim + 'real_request'
					else:
						print_format_2 += para_delim + '*' + true_para.lstrip("*")
				else:
					print_format_2 += para_delim + true_para.lstrip('*')

		elif pointer == False and array == True:
			true_para = true_para.rstrip('[]')
			true_para = true_para.strip('*')
			if func.endswith('array'):
				get_array_size = 'array_size = ndims;\n'
			else:
				get_array_size = 'array_size = count;\n'
			get_array_content += 's_offset += sprintf(' + true_para + '_arraystore+s_offset, "%d-", array_size);\n' 
			if type_of_para != 'MPI_Datatype':
				true_para = true_para.lstrip('*')
				get_array_content += 'for(int i=0; i<array_size; i++) {\n'
				get_array_content += '\ts_offset += sprintf(' + true_para + '_arraystore+s_offset, "' + para_format.get(type_of_para, 'Error') + '", ' + '*(' + true_para + '+i)' + ');\n'
				get_array_content += '\ts_offset += sprintf(' + true_para + '_arraystore+s_offset, ' + '"-");\n'
				get_array_content += '}\n'
				get_array_content += 's_offset = 0;\n'
			else:
				true_para = true_para.lstrip('*')
				get_array_content += 'for(int i=0; i<array_size; i++) {\n'
				get_array_content += 'temp_datatype = (' + true_para+'+i);'
				get_array_content += datatype_code('temp_datatype', True)

				get_array_content += '\ts_offset += sprintf(' + true_para + '_arraystore+s_offset, "%s", ' + 'temp_datatype_name'  +');\n'
				get_array_content += '\ts_offset += sprintf(' + true_para + '_arraystore+s_offset, ' + '"-");\n'
				get_array_content += '}\n'
				get_array_content += 's_offset = 0;\n'
				pass # TODO
			print_format_1 += true_para.lstrip('*') + '=' + "%s" + delim
			print_format_2 += para_delim + true_para.lstrip('*') + '_arraystore'
		else:
			print_format_1 += true_para.lstrip('*') + '=' + para_format.get(type_of_para, 'Error') + delim
			if type_of_para == 'MPI_File':
				print_format_2 += para_delim + true_para
			#	print_format_2 += para_delim + true_para
			else:
				print_format_2 += para_delim + true_para
	
	#print_format_1 = print_format_1[:-1]
	print_format_1 += '\\n"' + para_delim

			
	call_para = call_para[:-2]
	call_para += ')'

	body += '\t/* tm1 is the length of time elapsed between the IO or MPI communication calls. It\'s spend in computing */\n'
	body += '\ttm1 = recorder_wtime();\n'
	body += '\tcompute_time = tm1 - tm4;\n'
	body += '\tcompute_time_all += compute_time;\n'
	body += extra_data
	body += get_array_size
	body += get_array_content
	body += '\ttm2 = recorder_wtime();\n'
	body += '\tret = RECORDER_MPI_CALL(' + para_list[0] + ')' + call_para + ';\n'
	body += '\ttm3 = recorder_wtime();\n'
	body += '\tmpi_time = tm3 - tm2;\n'
#	body += '\tmpi_time_all += mpi_time;\n'
	if "read" in func:
		body += '\tread_time_all += mpi_time;\n'
	elif "write" in func:
		body += '\twrite_time_all += mpi_time;\n'
	else:
		body += '\tmpi_time_all += mpi_time;\n';
	body += after_call

#	body += '\tif (__recorderfh != NULL)\n'
	#body += '\tbytes = sprintf(rec_buffer + written_bytes, ' + print_format_1 + delim + print_format_2 + ');\n'
	body += '\tbytes = fprintf(__recorderfh, ' + print_format_1 + delim + print_format_2 + ');\n'
	#body += '\tread_func(sc, rec_buffer);\n'
	#body += '\twrite_or_compress(sc);\n'

	''' 
	does not need buffer here
	body += "\twritten_bytes += bytes;\n"
	body += "\tif (written_bytes > threshold) {\n"
	body += "\t\tfputs(rec_buffer, __recorderfh);\n"
	body += "\t\twritten_bytes = 0;\n}\n\n"
	'''

	body += '\tthis_func_time = recorder_wtime() - tm1;\n'
	body += '\trecorder_time_all += this_func_time - mpi_time;\n;'
	body += '\t/* tm4 stands for the true exit moment of this function */\n'
	body += '\ttm4 = recorder_wtime();\n'

	body += '\treturn ret;\n'
	body += '}\n\n'

	result = head + body
	out_file.write(result)
	func_file.write(func_info + '\n')
	return 0
		

def process(line, goodbye, out_file, func_file):
	recorder_prefix = 'RECORDER_FORWARD_DECL'
	# print line	
	if line.startswith(recorder_prefix) == False:
		return 1
	
	#strip prefix and parathens
	striped = line[len(recorder_prefix):-2]
	
	if striped.startswith(goodbye):
		# print "!!!!!!!!!!! Bye !!!!!!!!!!"
		return 2

	# print striped
	para_list = []
	temp = striped.split(',')
	# print temp
	for para in temp:
		bone = para.strip(' ()')
		if len(bone) > 0:
			para_list += [bone]

	generate_one_function(para_list, out_file, func_file)

	return 0

if __name__ == '__main__':
	input_file = open(sys.argv[1])
	output_file = open(sys.argv[2], 'w')
	input_header = open('_INC/input_header_compress.c')

	func_file = open(sys.argv[3], 'w')

	generate_head(input_header, output_file)
	for line in input_file:
		if process(line, '(PMPI_Finalize', output_file, func_file) == 2:
			break

	input_file.close()
	output_file.close()
	func_file.close()
