// Encrypted file Data Block info struct
typedef struct t_Block_Data {
	u32 size;               // Size of data block 
	u32 flags;              // Flags : 3 -> block to decrypt, 0 -> block to take in input file, 2 -> block to prepare
	u8 checksum[8];
} Block_Data_t;

// Encrypted file BIT table struct 
typedef struct t_Bit_Data {
	u32 headersize;         // KELF header size (same as KELF_Header_t.KELF_header_size)
	u8 block_count;         // Number of blocks in the KELF file 
	u8 pad1;
	u8 pad2;
	u8 pad3;
	Block_Data_t blocks[63];// KELF blocks infos
} Bit_Data_t;

// Encrypted file header struct 
typedef struct KELF_Header{
	u32 unknown1;		/* It's 0x01 0x00 0x00 0x01 */        
	u16 unknown2_half;	/* It's 0x00 0x03 */
	u8 region;		/* 0x00 - Japan, 0x01 - USA, 0x02 - Europe, 0x03 - Oceania, 0x04 - Asia, 0x05 - Russia, 0x07 - China, 0x09 - Middle east. */
	u8 language;		/* J, U, E, A, or C. */
	u16 unknown3_half;
	u16 version;
	u32 unknown4;
	u32 ELF_size;		/* Size of data blocks = Decrypted ELF size	*/
	u16 KELF_header_size;	/* KELF header size				*/
	u16 unknown5;
	u16 flags;
	u16 BIT_count;		/* used to set/get kbit and kc offset */
	u32 mg_zones;		/* 0x01 - Japan, 0x02 - USA, 0x04 - Europe, 0x08 - Oceania, 0x10 - Asia, 0x20 - Russia, 0x40 - China, 0x80 - Middle east. */
} KELF_Header_t;

struct RPC_AlignmentData{
	void *buffer1Address;
	void *buffer2Address;

	unsigned char buffer1[16];
	unsigned char buffer2[16];

	unsigned int buffer1_len;
	unsigned int buffer2_len;
};

void InitSECRMAN(void);
void DeinitSECRMAN(void);

typedef int (*McCommandHandler)(int port, int slot, void* sio2_trans_data);
typedef int (*McDevIDHandler)(int port, int slot);

/* 06 */ extern int SecrAuthCard(int port, int slot, int cnum);
/* 07 */ extern int SecrResetAuthCard(int port, int slot, int cnum);
/* 08 */ extern int SecrCardBootHeader(int port, int slot, void *buf, void *bit, int *psize, int size);
/* 09 */ extern int SecrCardBootBlock(void *src, void *dst, int size);
/* 10 */ extern void *SecrCardBootFile(int port, int slot, void *buf, int size);
/* 11 */ extern int SecrDiskBootHeader(void *buf, void *bit, int *psize, int size);
/* 12 */ extern int SecrDiskBootBlock(void *src, void *dst, int size);
/* 13 */ extern void *SecrDiskBootFile(void *buf, int size);

/* FOLLOWING EXPORTS ARE ONLY AVAILABLE IN SPECIAL SECRMAN OR FREESECR */ 

/* 14 */ extern int SecrDownloadHeader(int port, int slot, void *buf, void *bit, int *psize, int size);
/* 15 */ extern int SecrDownloadBlock(void *src, int size);
/* 16 */ extern void *SecrDownloadFile(int port, int slot, void *buf, int size);
/* 17 */ extern int SecrDownloadGetKbit(int port, int slot, void *kbit);
/* 18 */ extern int SecrDownloadGetKc(int port, int slot, void *kc);
/* 19 */ extern int SecrDownloadGetICVPS2(void *icvps2);

/* RPC function numbers */
#define SECRMAN_SetMcCommandHandler	0x01
#define SECRMAN_SetMcDevIDHandler	0x02
#define SECRMAN_SecrAuthCard		0x03
#define SECRMAN_SecrResetAuthCard	0x04
#define SECRMAN_SecrCardBootHeader	0x05
#define SECRMAN_SecrCardBootBlock	0x06
#define SECRMAN_SecrCardBootFile	0x07
#define SECRMAN_SecrDiskBootHeader	0x08
#define SECRMAN_SecrDiskBootBlock	0x09
#define SECRMAN_SecrDiskBootFile	0x0A
#define SECRMAN_SecrDownloadHeader	0x0B
#define SECRMAN_SecrDownloadBlock	0x0C
#define SECRMAN_SecrDownloadFile	0x0D
#define SECRMAN_SecrDownloadGetKbit	0x0E
#define SECRMAN_SecrDownloadGetKc	0x0F
#define SECRMAN_SecrDownloadGetICVPS2	0x10
#define SECRMAN_Init			0x80

