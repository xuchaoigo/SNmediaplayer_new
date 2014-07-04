#include "mp3_parse.h"

/****************************************************************/


/* BITRATE[version][layer][index] */
const int BITRATE[2][4][16]=
{ /*	0000,0001,0010,0011,0100,0101,0110,0111,1000,1001,1010,1011,1100,1101,1110,1111				*/
	{{ 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },    /* MPEG 2 -	*/
	 { 0,   8,  16,  24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160,   0 },    /* MPEG 2 Layer 3	*/
	 { 0,   8,  16,  24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160,   0 },    /* MPEG 2 Layer 2	*/
	 { 0,  32,  48,  56,  64,  80,  96, 112, 128, 144, 160, 176, 192, 224, 256,   0 }},   /* MPEG 2 Layer 1	*/
	{{ 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 },    /* MPEG 1 -	*/
	 { 0,  32,  40,  48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320,   0 },    /* MPEG 1 Layer 3	*/
	 { 0,  32,  48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 384,   0 },    /* MPEG 1 Layer 2	*/
	 { 0,  32,  64,  96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448,   0	}}    /* MPEG 1 Layer 1	*/
};

const int SAMPLING[2][2][4]=
{ /*	00	01	10	11 					*/
     {{	11025,	12000,	 8000,	0 },	/* MPEG 2 Extension (MPEG 2.5)	*/
      {	    0,	    0,	    0,	0 }},	/* -				*/
     {{	22050,	24000,	16000,	0 },	/* MPEG 2			*/
      {	44100,	48000,	32000,	0 }}	/* MPEG 1 			*/
};

void parse_mp3header(mpeg_frame_struct *frame) 
{

	int bitoffset=11;

	if((frame->header&0xffe00000)!=0xffe00000) {
		frame->valid=0;
		return;
	}

	frame->IDex=		bits(frame->header, 32, bitoffset, 1); /* 1:MPEG 1/MPEG 2, 0:MPEG 2.5 */
	frame->ID=		bits(frame->header, 32, bitoffset, 1); /* 1:MPEG 1, 0:MPEG 2 */
	frame->layer=		bits(frame->header, 32, bitoffset, 2); /* 1:Layer 3, 2:Layer 2, 3:Layer 1 */
	frame->protection_bit=	bits(frame->header, 32, bitoffset, 1); /* 0:CRC */
	frame->bitrate_index=	bits(frame->header, 32, bitoffset, 4);
	frame->sampling_frequency=bits(frame->header,32,bitoffset, 2);
	frame->padding_bit=	bits(frame->header, 32, bitoffset, 1); /* 1:Padding bit */
	frame->private_bit=	bits(frame->header, 32, bitoffset, 1);
	frame->mode=		bits(frame->header, 32, bitoffset, 2);
	frame->mode_extension=	bits(frame->header, 32, bitoffset, 2);
	frame->copyright=	bits(frame->header, 32, bitoffset, 1);
	frame->original=	bits(frame->header, 32, bitoffset, 1);
	frame->emphasis=	bits(frame->header, 32, bitoffset, 2);

	frame->bitrate=BITRATE[frame->ID][frame->layer][frame->bitrate_index];
	frame->sampling=SAMPLING[frame->IDex][frame->ID][frame->sampling_frequency];

	frame->valid=frame->bitrate&&frame->sampling&&frame->emphasis!=2;

	if(frame->valid) {
		switch(frame->layer) {
			case  3: frame->length=((12000*frame->bitrate/frame->sampling)+frame->padding_bit)<<2; break;	/* Layer 1 */
			case  2:											/* Layer 2 */
			case  1: frame->length=144000*frame->bitrate/frame->sampling;					/* Layer 3 */
				 if(!frame->ID) frame->length>>=1; /* MPEG 2.x */
				 frame->length+=frame->padding_bit; break;
		}
	} else frame->length=0;

#if 0
	printf("frame->header=%x\n",frame->header);
	printf("frame->IDex=%d\n",frame->IDex);
	printf("frame->ID=%d\n",frame->ID);
	printf("frame->layer=%d\n",frame->layer);
	printf("frame->protection_bit=%d\n",frame->protection_bit);
	printf("frame->bitrate_index=%d\n",frame->bitrate_index);
	printf("frame->sampling_frequency=%d\n",frame->sampling_frequency);
	printf("frame->padding_bit=%d\n",frame->padding_bit);
	printf("frame->private_bit=%d\n",frame->private_bit);
	printf("frame->mode=%d\n",frame->mode);
	printf("frame->mode_extension=%d\n",frame->mode_extension);
	printf("frame->copyright=%d\n",frame->copyright);
	printf("frame->original=%d\n",frame->original);
	printf("frame->emphasis=%d\n",frame->emphasis);
	printf("frame->bitrate=%d\n", frame->bitrate);
	printf("frame->sampling=%d\n", frame->sampling);
	printf("frame->length=%d\n", frame->length);
	printf("frame->valid=%d\n", frame->valid);
#endif

	return;
}
/************************************/

