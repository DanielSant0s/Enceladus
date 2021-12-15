#include <iopheap.h>
#include <kernel.h>
#include <sifrpc.h>
#include <string.h>
#include <errno.h>
#include "include/secrman_rpc.h"

static SifRpcClientData_t SECRMAN_rpc_cd;
static struct RPC_AlignmentData BufferAlignmentData ALIGNED(16);
static unsigned char ReceiveBuffer[64] ALIGNED(64);
static unsigned int TransmitBuffer[16] ALIGNED(64);

static void SECRMAN_RpcEndFunc(void *AlignmentData){
	memcpy(((struct RPC_AlignmentData *)UNCACHED_SEG(AlignmentData))->buffer1Address, ((struct RPC_AlignmentData *)UNCACHED_SEG(AlignmentData))->buffer1, ((struct RPC_AlignmentData *)UNCACHED_SEG(AlignmentData))->buffer1_len);
	memcpy(((struct RPC_AlignmentData *)UNCACHED_SEG(AlignmentData))->buffer2Address, ((struct RPC_AlignmentData *)UNCACHED_SEG(AlignmentData))->buffer2, ((struct RPC_AlignmentData *)UNCACHED_SEG(AlignmentData))->buffer2_len);
}

void InitSECRMAN(void){
	while((SifBindRpc(&SECRMAN_rpc_cd, 0x80000A00, 0)<0) || (SECRMAN_rpc_cd.server == NULL)) nopdelay();

	TransmitBuffer[0]=(unsigned int)&BufferAlignmentData;
	SifCallRpc(&SECRMAN_rpc_cd, SECRMAN_Init, 0, TransmitBuffer, sizeof(TransmitBuffer), NULL, 0, NULL, NULL);
}

void DeinitSECRMAN(void){
	memset(&SECRMAN_rpc_cd, 0, sizeof(SifRpcClientData_t));
}

int SecrAuthCard(int port, int slot, int cnum){
	int RPC_res;

	TransmitBuffer[0]=port;
	TransmitBuffer[1]=slot;
	TransmitBuffer[2]=cnum;

	if((RPC_res=SifCallRpc(&SECRMAN_rpc_cd, SECRMAN_SecrAuthCard, 0, TransmitBuffer, sizeof(TransmitBuffer), ReceiveBuffer, sizeof(int), NULL, NULL))>=0) RPC_res=*(int *)ReceiveBuffer;

	return RPC_res;
}

int SecrResetAuthCard(int port, int slot, int cnum){
	int RPC_res;

	TransmitBuffer[0]=port;
	TransmitBuffer[1]=slot;
	TransmitBuffer[2]=cnum;

	if((RPC_res=SifCallRpc(&SECRMAN_rpc_cd, SECRMAN_SecrResetAuthCard, 0, TransmitBuffer, sizeof(TransmitBuffer), ReceiveBuffer, sizeof(int), NULL, NULL))>=0) RPC_res=*(int *)ReceiveBuffer;

	return RPC_res;
}

int SecrCardBootHeader(int port, int slot, void *buf, void *bit, int *psize, int size){
	int DMATransferID, RPC_res;
	SifDmaTransfer_t dmat;

	dmat.src=buf;
	dmat.dest=SifAllocIopHeap(size);
	dmat.size=size;
	dmat.attr=0;

	if(dmat.dest!=NULL){
		SifWriteBackDCache(buf, size);
		DMATransferID=SifSetDma(&dmat, 1);

		/* Do something useful here. */
		TransmitBuffer[0]=port;
		TransmitBuffer[1]=slot;
		TransmitBuffer[2]=(int)dmat.dest;
		TransmitBuffer[3]=(int)bit;
		TransmitBuffer[4]=(int)psize;
		TransmitBuffer[5]=(int)buf;
		TransmitBuffer[6]=(int)size;

		while(SifDmaStat(DMATransferID)>=0){};

		if((RPC_res=SifCallRpc(&SECRMAN_rpc_cd, SECRMAN_SecrCardBootHeader, 0, TransmitBuffer, sizeof(TransmitBuffer), ReceiveBuffer, sizeof(int), &SECRMAN_RpcEndFunc, &BufferAlignmentData))>=0) RPC_res=*(int *)ReceiveBuffer;

		SifFreeIopHeap(dmat.dest);
	}
	else RPC_res=-ENOMEM;

	return RPC_res;
}

int SecrCardBootBlock(void *src, void *dst, int size){
	int DMATransferID, RPC_res;
	SifDmaTransfer_t dmat;

	dmat.src=src;
	dmat.dest=SifAllocIopHeap(size);
	dmat.size=size;
	dmat.attr=0;

	if(dmat.dest!=NULL){
		SifWriteBackDCache(src, size);
		DMATransferID=SifSetDma(&dmat, 1);

		TransmitBuffer[0]=(int)dmat.dest;
		TransmitBuffer[1]=TransmitBuffer[0];
		TransmitBuffer[2]=size;
		TransmitBuffer[3]=(int)dst;

		if((RPC_res=SifCallRpc(&SECRMAN_rpc_cd, SECRMAN_SecrCardBootBlock, 0, TransmitBuffer, sizeof(TransmitBuffer), ReceiveBuffer, sizeof(int), &SECRMAN_RpcEndFunc, &BufferAlignmentData))>=0) RPC_res=*(int *)ReceiveBuffer;

		SifFreeIopHeap(dmat.dest);
	}
	else RPC_res=-ENOMEM;

	return RPC_res;
}

void *SecrCardBootFile(int port, int slot, void *buf, int size){
	int DMATransferID;
	void *result;
	SifDmaTransfer_t dmat;

	result=NULL;
	if((dmat.dest=SifAllocIopHeap(size))!=NULL){
		dmat.src=buf;
		dmat.size=size;
		dmat.attr=0;
		SifWriteBackDCache(buf, size);
		while((DMATransferID=SifSetDma(&dmat, 1))==0){};

		/* Do something useful here. */
		TransmitBuffer[0]=port;
		TransmitBuffer[1]=slot;
		TransmitBuffer[2]=(int)dmat.dest;
		TransmitBuffer[3]=(int)buf;
		TransmitBuffer[4]=size;

		while(SifDmaStat(DMATransferID)>=0){};

		result=(SifCallRpc(&SECRMAN_rpc_cd, SECRMAN_SecrCardBootFile, 0, TransmitBuffer, sizeof(TransmitBuffer), ReceiveBuffer, sizeof(void *), &SECRMAN_RpcEndFunc, &BufferAlignmentData)<0)?NULL:*(void **)ReceiveBuffer;
		SifFreeIopHeap(dmat.dest);
	}

	return result;
}

int SecrDiskBootHeader(void *buf, void *bit, int *psize, int size){
	int DMATransferID, RPC_res;
	SifDmaTransfer_t dmat;

	if((dmat.dest=SifAllocIopHeap(size))!=NULL){
		dmat.src=buf;
		dmat.size=size;
		dmat.attr=0;
		SifWriteBackDCache(buf, size);
		while((DMATransferID=SifSetDma(&dmat, 1))==0){};

		/* Do something useful here. */
		TransmitBuffer[0]=(int)dmat.dest;
		TransmitBuffer[1]=(int)bit;
		TransmitBuffer[2]=(int)psize;
		TransmitBuffer[3]=size;

		while(SifDmaStat(DMATransferID)>=0){};

		if((RPC_res=SifCallRpc(&SECRMAN_rpc_cd, SECRMAN_SecrDiskBootHeader, 0, TransmitBuffer, sizeof(TransmitBuffer), ReceiveBuffer, sizeof(int), &SECRMAN_RpcEndFunc, &BufferAlignmentData))>=0) RPC_res=*(int *)ReceiveBuffer;

		SifFreeIopHeap(dmat.dest);
	}
	else RPC_res=-ENOMEM;

	return RPC_res;
}

int SecrDiskBootBlock(void *src, void *dst, int size){
	int DMATransferID, RPC_res;
	SifDmaTransfer_t dmat;

	dmat.src=src;
	dmat.dest=SifAllocIopHeap(size);
	dmat.size=size;
	dmat.attr=0;

	if(dmat.dest!=NULL){
		SifWriteBackDCache(src, size);
		DMATransferID=SifSetDma(&dmat, 1);

		TransmitBuffer[0]=(int)dmat.dest;
		TransmitBuffer[1]=TransmitBuffer[0];
		TransmitBuffer[2]=size;
		TransmitBuffer[3]=(int)dst;

		if((RPC_res=SifCallRpc(&SECRMAN_rpc_cd, SECRMAN_SecrDiskBootBlock, 0, TransmitBuffer, sizeof(TransmitBuffer), ReceiveBuffer, sizeof(int), &SECRMAN_RpcEndFunc, &BufferAlignmentData))>=0) RPC_res=*(int *)ReceiveBuffer;

		SifFreeIopHeap(dmat.dest);
	}
	else RPC_res=-ENOMEM;

	return RPC_res;
}

void *SecrDiskBootFile(void *buf, int size){
	int DMATransferID;
	void *RPC_res;
	SifDmaTransfer_t dmat;

	RPC_res=NULL;
	if((dmat.dest=SifAllocIopHeap(size))!=NULL){
		dmat.src=buf;
		dmat.size=size;
		dmat.attr=0;
		SifWriteBackDCache(buf, size);
		while((DMATransferID=SifSetDma(&dmat, 1))==0){};

		/* Do something useful here. */
		TransmitBuffer[0]=(int)dmat.dest;
		TransmitBuffer[1]=(int)buf;
		TransmitBuffer[2]=size;

		while(SifDmaStat(DMATransferID)>=0){};

		RPC_res=(SifCallRpc(&SECRMAN_rpc_cd, SECRMAN_SecrDiskBootFile, 0, TransmitBuffer, sizeof(TransmitBuffer), ReceiveBuffer, sizeof(void*), &SECRMAN_RpcEndFunc, &BufferAlignmentData)<0)?NULL:*(void **)ReceiveBuffer;

		SifFreeIopHeap(dmat.dest);
	}

	return RPC_res;
}

int SecrDownloadHeader(int port, int slot, void *buf, void *bit, int *psize, int size){
	int DMATransferID, RPC_res;
	SifDmaTransfer_t dmat;

	dmat.src=buf;
	dmat.dest=SifAllocIopHeap(size);
	dmat.size=size;
	dmat.attr=0;

	if(dmat.dest!=NULL){
		SifWriteBackDCache(buf, size);
		DMATransferID=SifSetDma(&dmat, 1);

		/* Do something useful here. */
		TransmitBuffer[0]=port;
		TransmitBuffer[1]=slot;
		TransmitBuffer[2]=(int)dmat.dest;
		TransmitBuffer[3]=(int)bit;
		TransmitBuffer[4]=(int)psize;
		TransmitBuffer[5]=(int)buf;
		TransmitBuffer[6]=(int)size;

		while(SifDmaStat(DMATransferID)>=0){};

		if((RPC_res=SifCallRpc(&SECRMAN_rpc_cd, SECRMAN_SecrDownloadHeader, 0, TransmitBuffer, sizeof(TransmitBuffer), ReceiveBuffer, sizeof(int), &SECRMAN_RpcEndFunc, &BufferAlignmentData))>=0) RPC_res=*(int *)ReceiveBuffer;

		SifFreeIopHeap(dmat.dest);
	}
	else RPC_res=-ENOMEM;

	return RPC_res;
}

int SecrDownloadBlock(void *src, int size){
	int DMATransferID, RPC_res;
	SifDmaTransfer_t dmat;

	RPC_res=-1;
	if((dmat.dest=SifAllocIopHeap(size))!=NULL){
		dmat.src=src;
		dmat.size=size;
		dmat.attr=0;
		SifWriteBackDCache(src, size);
		while((DMATransferID=SifSetDma(&dmat, 1))==0){};

		/* Do something useful here. */
		TransmitBuffer[0]=(int)dmat.dest;
		TransmitBuffer[1]=size;
		TransmitBuffer[2]=(int)src;

		while(SifDmaStat(DMATransferID)>=0){};

		if((RPC_res=SifCallRpc(&SECRMAN_rpc_cd, SECRMAN_SecrDownloadBlock, 0, TransmitBuffer, sizeof(TransmitBuffer), ReceiveBuffer, sizeof(int), &SECRMAN_RpcEndFunc, &BufferAlignmentData))>=0) RPC_res=*(int *)ReceiveBuffer;
		SifFreeIopHeap(dmat.dest);
	}

	return RPC_res;
}

void *SecrDownloadFile(int port, int slot, void *buf, int size){
	int DMATransferID;
	void *result;
	SifDmaTransfer_t dmat;

	result=NULL;
	if((dmat.dest=SifAllocIopHeap(size))!=NULL){
		dmat.src=buf;
		dmat.size=size;
		dmat.attr=0;

		SifWriteBackDCache(buf, size);
		while((DMATransferID=SifSetDma(&dmat, 1))==0){};

		/* Do something useful here. */
		TransmitBuffer[0]=port;
		TransmitBuffer[1]=slot;
		TransmitBuffer[2]=(int)dmat.dest;
		TransmitBuffer[3]=(int)buf;
		TransmitBuffer[4]=size;

		while(SifDmaStat(DMATransferID)>=0){};

		result=(SifCallRpc(&SECRMAN_rpc_cd, SECRMAN_SecrDownloadFile, 0, TransmitBuffer, sizeof(TransmitBuffer), ReceiveBuffer, sizeof(void *), &SECRMAN_RpcEndFunc, &BufferAlignmentData)<0)?NULL:*(void **)ReceiveBuffer;

		SifFreeIopHeap(dmat.dest);
	}

	return result;
}

int SecrDownloadGetKbit(int port, int slot, void *kbit){
	int RPC_res;

	TransmitBuffer[0]=port;
	TransmitBuffer[1]=slot;
	TransmitBuffer[2]=(int)kbit;

	SifWriteBackDCache(kbit, 16);

	if((RPC_res=SifCallRpc(&SECRMAN_rpc_cd, SECRMAN_SecrDownloadGetKbit, 0, TransmitBuffer, sizeof(TransmitBuffer), ReceiveBuffer, sizeof(int), NULL, NULL))>=0) RPC_res=*(int *)ReceiveBuffer;

	return RPC_res;
}

int SecrDownloadGetKc(int port, int slot, void *kc){
	int RPC_res;

	TransmitBuffer[0]=port;
	TransmitBuffer[1]=slot;
	TransmitBuffer[2]=(int)kc;

	SifWriteBackDCache(kc, 16);

	if((RPC_res=SifCallRpc(&SECRMAN_rpc_cd, SECRMAN_SecrDownloadGetKc, 0, TransmitBuffer, sizeof(TransmitBuffer), ReceiveBuffer, sizeof(int), NULL, NULL))>=0) RPC_res=*(int *)ReceiveBuffer;

	return RPC_res;
}

int SecrDownloadGetICVPS2(void *icvps2){
	int RPC_res;

	SifWriteBackDCache(icvps2, 8);

	if((RPC_res=SifCallRpc(&SECRMAN_rpc_cd, SECRMAN_SecrDownloadGetICVPS2, 0, &icvps2, sizeof(void*), ReceiveBuffer, sizeof(int), NULL, NULL))>=0) RPC_res=*(int *)ReceiveBuffer;

	return RPC_res;
}
