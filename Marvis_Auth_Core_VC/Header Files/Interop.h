#include <stdio.h>

extern "C" {
	int __cdecl comp_nfiq(int *, float *, unsigned char *, const int, const int, const int, const int, int *);
	int __cdecl read_and_decode_grayscale_image(char *, int *, unsigned char **, int *, int *, int *, int *, int *);
	int __cdecl wsq_decode_mem(unsigned char **odata, int *ow, int *oh, int *od, int *oppi, int *lossyflag, unsigned char *idata, const int ilen);
	int __cdecl wsq_encode_mem(unsigned char **odata, int *olen, const float r_bitrate, unsigned char *idata, const int w, const int h, const int d, const int ppi, char *comment_text);
	
	struct seg_rec_coords {
		int tlx, tly;
		int trx, tRy;
		int blx, bly;
		int brx, bry;
		int sx, sy, sw, sh, nrsw, nrsh;
		float theta;
		int dty, dby, dlx, drx;
		int err;
	};

	int __cdecl segment_fingers(unsigned char *, const int, const int, seg_rec_coords **, const int, const int);
	int __cdecl parse_segfing(unsigned char ***pdata, unsigned char *data, const int w, const int h, seg_rec_coords *fing_boxes, const int nf, const int rot);

	
	struct minutia {
		int x;
		int y;
		int ex;
		int ey;
		int direction;
		double reliability;
		int type;
		int appearing;
		int feature_id;
		int *nbrs;
		int *ridge_counts;
		int num_nbrs;
	};

	struct minutiae {
		int alloc;
		int num;
		minutia **list;
	};

	struct lfsparms{
   /* Image Controls */
   int    pad_value;
   int    join_line_radius;

   /* Map Controls */
   int    blocksize;       /* Pixel dimension image block.                 */
   int    windowsize;      /* Pixel dimension window surrounding block.    */
   int    windowoffset;    /* Offset in X & Y from block to window origin. */
   int    num_directions;
   double start_dir_angle;
   int    rmv_valid_nbr_min;
   double dir_strength_min;
   int    dir_distance_max;
   int    smth_valid_nbr_min;
   int    vort_valid_nbr_min;
   int    highcurv_vorticity_min;
   int    highcurv_curvature_min;
   int    min_interpolate_nbrs;
   int    percentile_min_max;
   int    min_contrast_delta;

   /* DFT Controls */
   int    num_dft_waves;
   double powmax_min;
   double pownorm_min;
   double powmax_max;
   int    fork_interval;
   double fork_pct_powmax;
   double fork_pct_pownorm;

   /* Binarization Controls */
   int    dirbin_grid_w;
   int    dirbin_grid_h;
   int    isobin_grid_dim;
   int    num_fill_holes;

   /* Minutiae Detection Controls */
   int    max_minutia_delta;
   double max_high_curve_theta;
   int    high_curve_half_contour;
   int    min_loop_len;
   double min_loop_aspect_dist;
   double min_loop_aspect_ratio;

   /* Minutiae Link Controls */
   int    link_table_dim;
   int    max_link_dist;
   int    min_theta_dist;
   int    maxtrans;
   double score_theta_norm;
   double score_dist_norm;
   double score_dist_weight;
   double score_numerator;

   /* False Minutiae Removal Controls */
   int    max_rmtest_dist;
   int    max_hook_len;
   int    max_half_loop;
   int    trans_dir_pix;
   int    small_loop_len;
   int    side_half_contour;
   int    inv_block_margin;
   int    rm_valid_nbr_min;
   int    max_overlap_dist;
   int    max_overlap_join_dist;
   int    malformation_steps_1;
   int    malformation_steps_2;
   double min_malformation_ratio;
   int    max_malformation_dist;
   int    pores_trans_r;
   int    pores_perp_steps;
   int    pores_steps_fwd;
   int    pores_steps_bwd;
   double pores_min_dist2;
   double pores_max_ratio;

   /* Ridge Counting Controls */
   int    max_nbrs;
   int    max_ridge_steps;
};
	
	int debug;
	extern lfsparms lfsparms_V2;
	int __cdecl get_minutiae(minutiae **, int **, int **, int **,
                 int **, int **, int *, int *,
                 unsigned char **, int *, int *, int *,
                 unsigned char *, const int, const int,
                 const int, const double, const lfsparms *);
	

	/* BOZORTH SUPPORT */

	#define DEFAULT_BOZORTH_MINUTIAE   150
	#define MAX_BOZORTH_MINUTIAE		200
	#define MAX_FILE_MINUTIAE		1000
	#define COLS_SIZE_2 6
	//#define MAX_HUFFBITS 16

	struct xyt_struct {
		int nrows;
		int xcol[     MAX_BOZORTH_MINUTIAE];
		int ycol[     MAX_BOZORTH_MINUTIAE];
		int thetacol[ MAX_BOZORTH_MINUTIAE];
	};

	struct xytq_struct {
		int nrows;
		int xcol[       MAX_FILE_MINUTIAE ];
		int ycol[       MAX_FILE_MINUTIAE ];
		int thetacol[   MAX_FILE_MINUTIAE ];
		int qualitycol[ MAX_FILE_MINUTIAE ];
	};

	FILE *errorfp;
	int m1_xyt;
	int max_minutiae = DEFAULT_BOZORTH_MINUTIAE;
	int min_computable_minutiae;
	int verbose_main;
	int verbose_load;
	int verbose_bozorth;

	int __cdecl bozorth_probe_init(struct xyt_struct *);
	int __cdecl bozorth_gallery_init(struct xyt_struct *);
	int __cdecl bz_match(int, int);
	int __cdecl bz_match_score(int, struct xyt_struct *, struct xyt_struct *);
	int __cdecl bozorth_main(struct xyt_struct *, struct xyt_struct *);

	void __cdecl lfs2nist_format(minutiae *, int, int);
	xyt_struct* __cdecl bz_prune(struct xytq_struct *, int);

	/* END BOZORTH SUPPORT */







	

}