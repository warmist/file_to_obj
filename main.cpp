#include <cstdint>
#include <stdio.h>
#include <time.h>
#include <vector>
#pragma pack(push,1)
struct coff_header
{
	uint16_t machine;
	uint16_t no_sections;
	uint32_t time_stamp;
	uint32_t ptr_symbols;
	uint32_t no_symbols;
	uint16_t opt_header_size;
	uint16_t flags;
};

struct section_header
{
	char name[8];
	uint32_t vsize;
	uint32_t vaddr;
	uint32_t size_raw_data;
	uint32_t ptr_raw_data;
	uint32_t ptr_relocations;
	uint32_t ptr_line_nos;
	uint16_t no_relocations;
	uint16_t no_line_nos;
	uint32_t flags;
};
struct symbol {
	union {
		char name[8];
		struct {
			uint32_t name_zeroes;
			uint32_t name_offset;
		};
	};
	uint32_t value;
	int16_t scnum;
	uint16_t type;
	uint8_t sclass;
	uint8_t num_aux;
};
#pragma pack(pop)
struct symbol_loaded {
	std::string name;
};
enum section_flags {
	IMAGE_SCN_CNT_INITIALIZED_DATA = 0x00000040,
	IMAGE_SCN_ALIGN_8BYTES = 0x00400000,
	IMAGE_SCN_MEM_READ = 0x40000000,
};
#define CAST_VALUE(tname,data) *(reinterpret_cast<tname*>(data))
#if 0 //used to load/test the obj files to see how they work
void load_obj(char* fname)
{
	auto f = fopen(fname, "rb");

	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	std::vector<char> buffer(fsize);
	fread(buffer.data(), fsize, 1, f);
	fclose(f);

	auto hdr = CAST_VALUE(coff_header, buffer.data());

	time_t hdr_time = hdr.time_stamp;
	printf("Linked: %s", asctime(gmtime(&hdr_time)));
	auto ptr = buffer.data() + sizeof(coff_header);
	auto ptr_start = buffer.data();
	std::vector<std::pair<section_header, std::vector<char> >> shdrs(hdr.no_sections);

	for(auto& sh: shdrs)
	{
		sh.first = CAST_VALUE(section_header, ptr);
		ptr += sizeof(section_header);
		sh.second.resize(sh.first.size_raw_data);
		auto ptr_data = ptr_start + sh.first.ptr_raw_data;
		for (int i = 0; i < sh.first.size_raw_data; i++)
		{
			sh.second[i] = *ptr_data;
			ptr_data++;
		}
	}
	std::vector<std::pair<symbol, symbol_loaded>> symbols(hdr.no_symbols);
	auto ptr_symbol = buffer.data() + hdr.ptr_symbols;
	for(auto& s: symbols)
	{
		s.first= CAST_VALUE(symbol, ptr_symbol);
		ptr_symbol += sizeof(symbol);
	}
	auto ptr_strings = ptr_symbol;
	for (auto& s : symbols)
	{
		if (s.first.name_zeroes == 0)
		{
			size_t slen = strlen(ptr_strings + s.first.name_offset);
			s.second.name.resize(slen);
			strcpy(const_cast<char*>(s.second.name.data()), ptr_strings + s.first.name_offset);
		}
		else
		{
			int slen=strnlen(s.first.name, 8);
			s.second.name = std::string(s.first.name, slen);
		}
	}
	for (int i = 0; i < symbols.size(); i++)
	{
		auto& s = symbols[i];
		if (s.first.sclass == 2)
		{
			printf("%s :\n", s.second.name.c_str());
		}
	}
	fclose(f);
}
#endif
size_t align_value(size_t v, size_t alignment)
{
	int r = v % alignment;
	return r ? (v + (alignment - r)) : v;
}
bool write_symbol(FILE* f,const char* name,uint32_t data_offset,uint32_t& str_size)
{
	bool needs_string = false;
	symbol s = { 0 };
	s.scnum = 1;
	int slen = strlen(name);
	if (slen <= 8)
	{
		for (int i = 0; i < slen; i++)
			s.name[i] = name[i];
	}
	else
	{
		needs_string = true;
		s.name_zeroes = 0;
		s.name_offset = str_size+4;
		str_size += slen+1;
	}
	s.sclass = 2;
	s.value = data_offset;
	fwrite(&s, sizeof(s), 1, f);
	return needs_string;
}
void create_obj(const char* fname,const char* payload, size_t payload_size,const char* name_data,const char* name_size)
{
	uint32_t data_size= align_value(payload_size, 8) + align_value(sizeof(size_t), 8);
	//header
	coff_header hdr = { 0 };
	hdr.machine = 0x8664;
	hdr.no_sections = 1;
	hdr.no_symbols = 2;
	hdr.ptr_symbols = sizeof(coff_header) + sizeof(section_header) + data_size;
	time_t cur_time= time(NULL);
	hdr.time_stamp = (uint32_t)cur_time;

	auto f=fopen(fname, "wb");
	fwrite(&hdr, sizeof(hdr), 1, f);
	//sections
	section_header rdata = { 0 };
	strcpy(rdata.name, ".rdata");

	rdata.size_raw_data = data_size;
	rdata.flags = IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_ALIGN_8BYTES | IMAGE_SCN_MEM_READ;
	rdata.ptr_raw_data = sizeof(hdr) + sizeof(rdata);
	fwrite(&rdata, sizeof(rdata), 1, f);
	//actual rdata
	//now write the data
	fwrite(payload, payload_size, 1, f);
	//now write padding
	for(int i=0;i<align_value(payload_size, 8) - payload_size;i++)
		fputc(0, f);
	//write size of data
	fwrite((char*)&payload_size, sizeof(size_t), 1, f);
	//now write padding
	for (int i = 0; i<align_value(sizeof(size_t), 8) - sizeof(size_t); i++)
		fputc(0, f);
	//symbols
	uint32_t strings_size = 0;
	bool write_data_str = write_symbol(f, name_data, 0, strings_size);
	bool write_name_str = write_symbol(f, name_size, align_value(payload_size, 8), strings_size);
	strings_size += sizeof(strings_size);
	fwrite(&strings_size, sizeof(strings_size), 1, f);
	if (write_data_str)
		fwrite(name_data, strlen(name_data)+1, 1, f);
	if (write_name_str)
		fwrite(name_size, strlen(name_size)+1, 1, f);
	fclose(f);
}
void create_header(const char* fname, const char* name_data, const char* name_size)
{
	auto f = fopen(fname, "w");
	fprintf(f, "#pragma once\nextern \"C\" {\nextern const char %s[];\nextern const size_t %s;\n}\n", name_data, name_size);
	fclose(f);
}
void append_hex(std::string& trg, size_t value)
{
	if (value == 0)
		return;
	int rem = value % 16;
	value /= 16;
	append_hex(trg, value);

	trg.append(1,char(rem + 'A'));
}
std::string mangle_size(size_t s)
{
	std::string ret;
	/*
		number mangling:
			1<= N <= 10  | (N - 1) as a decimal number (in real case did not happen!)
			N > 10       | code N as a hexadecimal number without leading zeroes, replace the hexadecimal digits 0 - F by the letters A - P, end with a @
			N = 0		 | A@
			N < 0        | ? followed by above
	*/
	if (s == 0)
		return "A@";
	append_hex(ret, s);
	return ret;
}
//creates extern const std::array<char,[size]>={data};
void create_obj_cpp(const char* fname, const char* payload, size_t payload_size, const char* name_data)
{
	uint32_t data_size = align_value(payload_size, 8) ;
	//header
	coff_header hdr = { 0 };
	hdr.machine = 0x8664;
	hdr.no_sections = 1;
	hdr.no_symbols = 1;
	hdr.ptr_symbols = sizeof(coff_header) + sizeof(section_header) + data_size;
	time_t cur_time = time(NULL);
	hdr.time_stamp = (uint32_t)cur_time;

	auto f = fopen(fname, "wb");
	fwrite(&hdr, sizeof(hdr), 1, f);
	//sections
	section_header rdata = { 0 };
	strcpy(rdata.name, ".rdata");

	rdata.size_raw_data = data_size;
	rdata.flags = IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_ALIGN_8BYTES | IMAGE_SCN_MEM_READ;
	rdata.ptr_raw_data = sizeof(hdr) + sizeof(rdata);
	fwrite(&rdata, sizeof(rdata), 1, f);
	//actual rdata
	//now write the data
	fwrite(payload, payload_size, 1, f);
	//now write padding - probably not needed!
	for (int i = 0; i<align_value(payload_size, 8) - payload_size; i++)
		fputc(0, f);
	//symbols
	uint32_t strings_size = 0;
	char buffer[256] = { 0 };
	auto mangled_size = mangle_size(payload_size);
	sprintf(buffer, "?%s@@3V?$array@D$0%s@@std@@B", name_data, mangled_size.c_str());
	bool write_data_str = write_symbol(f, buffer, 0, strings_size);
	strings_size += sizeof(strings_size);
	fwrite(&strings_size, sizeof(strings_size), 1, f);
	if (write_data_str)
		fwrite(buffer, strlen(buffer) + 1, 1, f);
	fclose(f);
}
void create_header_cpp(const char* fname, const char* name_data,size_t size)
{
	auto f = fopen(fname, "w");
	fprintf(f, "#pragma once\n#include <array>\nextern const std::array<char,%lld> %s;\n",  size, name_data);
	fclose(f);
}
bool load_file(const char* fname,std::vector<char>& buffer)
{
	auto f = fopen(fname, "rb");
	if (!f)
		return false;
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	buffer.resize(fsize);
	fread(buffer.data(), fsize, 1, f);
	fclose(f);
	return true;
}

int main_cpp(int argc, char** argv)
{
	std::vector<char> payload_data;
	if (!load_file(argv[2], payload_data))
	{
		printf("Failed to load file:%s", argv[2]);
		return -1;
	}
	std::string fname = argv[4];
	std::string out_obj_path = std::string(argv[3]) + "/" + fname + ".obj";
	std::string out_header_path = std::string(argv[3]) + "/" + fname + ".hpp";

	create_obj_cpp(out_obj_path.c_str(), payload_data.data(), payload_data.size(), argv[5]);
	create_header_cpp(out_header_path.c_str(), argv[5], payload_data.size());
}
int main(int argc, char** argv)
{
	//TODO: make last 3 args optional, but that needs path splitting and stuff...

	if (argc < 6)
	{
		printf("Usage: [-cpp] <path_to_payload> <output_path> <out_file_name> <data_variable_name> [<data_size_name>]\n  last arg is only needed in non-cpp mode");
		return -1;
	}
	if (argv[1] == std::string("-cpp"))
	{
		return main_cpp(argc, argv);
	}
	std::vector<char> payload_data;
	if (!load_file(argv[1], payload_data))
	{
		printf("Failed to load file:%s", argv[1]);
		return -1;
	}
	std::string fname = argv[3];
	std::string out_obj_path = std::string(argv[2]) + "/" + fname + ".obj";
	std::string out_header_path = std::string(argv[2]) + "/" + fname + ".hpp";

	create_obj(out_obj_path.c_str(), payload_data.data(), payload_data.size(), argv[4], argv[5]);
	create_header(out_header_path.c_str(), argv[4], argv[5]);
	return 0;
}