#include <kernel/system/elf_loader.h>

#include <stdint.h>
#include <string.h>

extern void execute(uint32_t buf);

typedef uint16_t Elf32_Half;
typedef	uint32_t Elf32_Off;
typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Word;
typedef int32_t Elf32_Sword;

# define ELF_NIDENT	16

typedef struct elf_header {
	uint8_t		e_ident[ELF_NIDENT];
	Elf32_Half	e_type;
	Elf32_Half	e_machine;
	Elf32_Word	e_version;
	Elf32_Addr	e_entry;
	Elf32_Off	e_phoff;
	Elf32_Off	e_shoff;
	Elf32_Word	e_flags;
	Elf32_Half	e_ehsize;
	Elf32_Half	e_phentsize;
	Elf32_Half	e_phnum;
	Elf32_Half	e_shentsize;
	Elf32_Half	e_shnum;
	Elf32_Half	e_shstrndx;
} elf_header_t;

enum Elf_Ident {
	EI_MAG0		= 0, // 0x7F
	EI_MAG1		= 1, // 'E'
	EI_MAG2		= 2, // 'L'
	EI_MAG3		= 3, // 'F'
	EI_CLASS	= 4, // Architecture (32/64)
	EI_DATA		= 5, // Byte Order
	EI_VERSION	= 6, // ELF Version
	EI_OSABI	= 7, // OS Specific
	EI_ABIVERSION	= 8, // OS Specific
	EI_PAD		= 9  // Padding
};
 
# define ELFMAG0	0x7F // e_ident[EI_MAG0]
# define ELFMAG1	'E'  // e_ident[EI_MAG1]
# define ELFMAG2	'L'  // e_ident[EI_MAG2]
# define ELFMAG3	'F'  // e_ident[EI_MAG3]
 
# define ELFDATA2LSB	(1)  // Little Endian
# define ELFCLASS32	(1)  // 32-bit Architecture

enum Elf_Type {
	ET_NONE		= 0, // Unkown Type
	ET_REL		= 1, // Relocatable File
	ET_EXEC		= 2  // Executable File
};
 
# define EM_386		(3)  // x86 Machine Type
# define EV_CURRENT	(1)  // ELF Current Version

typedef struct {
	Elf32_Word		p_type;
	Elf32_Off		p_offset;
	Elf32_Addr		p_vaddr;
	Elf32_Addr		p_paddr;
	Elf32_Word		p_filesz;
	Elf32_Word		p_memsz;
	Elf32_Word		p_flags;
	Elf32_Word		p_align;
} elf_program_header_t;

int elf_check_file(elf_header_t* hdr) {
	if(!hdr) return 0;
	if(hdr->e_ident[EI_MAG0] != ELFMAG0) {
		printf("ELF Header EI_MAG0 incorrect.\n");
		return 0;
	}
	if(hdr->e_ident[EI_MAG1] != ELFMAG1) {
		printf("ELF Header EI_MAG1 incorrect.\n");
		return 0;
	}
	if(hdr->e_ident[EI_MAG2] != ELFMAG2) {
		printf("ELF Header EI_MAG2 incorrect.\n");
		return 0;
	}
	if(hdr->e_ident[EI_MAG3] != ELFMAG3) {
		printf("ELF Header EI_MAG3 incorrect.\n");
		return 0;
	}
	return 1;
}

int elf_check_supported(elf_header_t* hdr) {
	if(!elf_check_file(hdr)) {
		printf("Invalid ELF File.\n");
		return 0;
	}
	if(hdr->e_ident[EI_CLASS] != ELFCLASS32) {
		printf("Unsupported ELF File Class.\n");
		return 0;
	}
	if(hdr->e_ident[EI_DATA] != ELFDATA2LSB) {
		printf("Unsupported ELF File byte order.\n");
		return 0;
	}
	if(hdr->e_machine != EM_386) {
		printf("Unsupported ELF File target.\n");
		return 0;
	}
	if(hdr->e_ident[EI_VERSION] != EV_CURRENT) {
		printf("Unsupported ELF File version.\n");
		return 0;
	}
	if(hdr->e_type != ET_REL && hdr->e_type != ET_EXEC) {
		printf("Unsupported ELF File type.\n");
		return 0;
	}
	return 1;
}


void load_elf_from_buffer(const unsigned char* buf, int size) {
	elf_header_t* header = (elf_header_t*)buf;
	if (elf_check_supported(header)) {
		int i = 0;
		while (i < header->e_phnum) {
			elf_program_header_t* program_header = (elf_program_header_t*)(buf+header->e_phoff + (i * header->e_phentsize));
			memset(program_header->p_vaddr, 0, program_header->p_memsz + program_header->p_filesz);
			memcpy(program_header->p_vaddr, buf+program_header->p_offset, program_header->p_filesz);
			printf("Copyed program data to address: %d\n", program_header->p_vaddr);
			i ++;
		}
		elf_program_header_t* program_header = (elf_program_header_t*)(buf+header->e_phoff);
		printf("Executing at memory address: %d\n", program_header->p_vaddr);
		execute(program_header->p_vaddr + header->e_entry);
	}
}
