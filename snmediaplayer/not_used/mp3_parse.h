#ifndef _MP3_PARSE_H
#define _MP3_PARSE_H


#define bits(value,size,bit,nbits)	((value>>(size-(bit+=nbits)))&((1<<nbits)-1))

typedef struct {		      /* 118/126 */
	int part2_3_length;		/* 12 */
	int big_values;			/*  9 */
	int global_gain;		/*  8 */
	int scalefac_compress;		/* 4/9 */
	int window_switching_flag;	/*  1 */
	int block_type;			/*  2 */
	int mixed_block_flag;		/*  1 */
	int table_select[3];		/*  5 */
	int subblock_gain[3];		/*  3 */
	int region0_count;		/*  4 */
	int region1_count;		/*  3 */
	int preflag;			/* 1/- */
	int scalefac_scale;		/*  1 */
	int count1table_select;		/*  1 */
} granule_struct;


typedef struct {		  /* 256/136/136/72 */
	int main_data_begin;		/* 9/8 */
	int private_bits;	      /* 3/5/2/1 */
	int scfsi_band[2][4];		/* 1/- */
	granule_struct granule[2][2];

	int length;
} sideinfo_struct;

typedef struct {
	unsigned int header;			/* 32 */
	unsigned int crc;			/* 16 */
	sideinfo_struct sideinfo;
	unsigned char *main_data;

     /* int syncword			   11 */
	int IDex;			/*  1 */
	int ID;				/*  1 */
	int layer;			/*  2 */
	int protection_bit;		/*  1 */
	int bitrate_index;		/*  4 */
	int sampling_frequency;		/*  2 */
	int padding_bit;		/*  1 */
	int private_bit;		/*  1 */
	int mode;			/*  2 */
	int mode_extension;		/*  2 */
	int copyright;			/*  1 */
	int original;			/*  1 */
	int emphasis;			/*  2 */

	int bitrate;
	int sampling;
	int length;
	int valid;
} mpeg_frame_struct;
void parse_mp3header(mpeg_frame_struct *frame); 


#endif
