#include "stdafx.h"

#include "blender_smaa.h"

CBlender_smaa::CBlender_smaa() { description.CLS = 0; }

CBlender_smaa::~CBlender_smaa()
{
}

void CBlender_smaa::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);
	switch (C.iElement)
	{
	case 0: //Edge detection
		C.r_Pass("pp_smaa_ed", "pp_smaa_ed", FALSE, FALSE, FALSE);
		C.r_dx10Texture("s_image", r2_RT_generic0);

		C.r_dx10Sampler("smp_nofilter");
		C.r_dx10Sampler("smp_rtlinear");
		C.r_End();
		break;
	case 1:	//Weight 
		C.r_Pass("pp_smaa_bc", "pp_smaa_bc", FALSE, FALSE, FALSE);	
		C.r_dx10Texture("s_image", r2_RT_generic0);
		C.r_dx10Texture("s_edgetex", r2_RT_smaa_edgetex);

		C.r_dx10Texture("s_areatex", "shaders\\smaa\\area_tex_dx11");
		C.r_dx10Texture("s_searchtex", "shaders\\smaa\\search_tex");

		C.r_dx10Sampler("smp_nofilter");
		C.r_dx10Sampler("smp_rtlinear");
		C.r_End();
		break;
	case 2:	//Blending
		C.r_Pass("pp_smaa_nb", "pp_smaa_nb", FALSE, FALSE, FALSE);	
		C.r_dx10Texture("s_image", r2_RT_generic0);
		C.r_dx10Texture("s_blendtex", r2_RT_smaa_blendtex);

		C.r_dx10Sampler("smp_nofilter");
		C.r_dx10Sampler("smp_rtlinear");	
		C.r_End();
		break;	
	}
}

CBlender_ssfx_taa::CBlender_ssfx_taa() { description.CLS = 0; }

CBlender_ssfx_taa::~CBlender_ssfx_taa()
{
}

void CBlender_ssfx_taa::Compile(CBlender_Compile& C)
{
	IBlender::Compile(C);
	switch (C.iElement)
	{
	case 0: // Motion Vectors
		C.r_Pass("stub_screen_space", "ssfx_taa_prepare", FALSE, FALSE, FALSE);

		
		C.r_dx10Texture("s_ssfx_taa", r2_RT_ssfx_taa); // rt_ssfx_taa
		C.r_dx10Texture("s_position", r2_RT_P);

		C.r_dx10Texture("s_motion_vectors", r2_RT_ssfx_motion_vectors);

		C.r_dx10Sampler("smp_point");
		C.r_dx10Sampler("smp_linear");
		C.r_dx10Sampler("smp_rtlinear");
		C.r_dx10Sampler("smp_nofilter");

		C.r_End();
		break;

	case 1: // TAA
		C.r_Pass("stub_screen_space", "ssfx_taa", FALSE, FALSE, FALSE);

		C.r_dx10Texture("s_current", r2_RT_generic0);
		C.r_dx10Texture("s_previous", r2_RT_ssfx_prev_frame);

		C.r_dx10Texture("s_motion_vectors", r2_RT_ssfx_taa);
		C.r_dx10Texture("s_mv", r2_RT_ssfx_motion_vectors);

		C.r_dx10Texture("s_depth", r2_RT_P);
		C.r_dx10Texture("s_previous_depth", r2_RT_ssfx_prevPos);

		C.r_dx10Texture("s_ssfx_temp", r2_RT_ssfx_temp); // 3DSS Scope Mask

		C.r_dx10Sampler("smp_point");
		C.r_dx10Sampler("smp_linear");
		C.r_dx10Sampler("smp_rtlinear");
		C.r_dx10Sampler("smp_nofilter");

		C.r_End();
		break;

	case 2: // TAA Sharp

		C.r_Pass("stub_screen_space", "ssfx_taa_sharp", FALSE, FALSE, FALSE);
		C.r_dx10Texture("s_current", r2_RT_ssfx_prev_frame); //rt_Generic_0

		C.r_dx10Texture("s_motion_vectors", r2_RT_ssfx_motion_vectors); // Debug

		C.r_dx10Sampler("smp_point");
		C.r_dx10Sampler("smp_linear");
		C.r_dx10Sampler("smp_rtlinear");
		C.r_dx10Sampler("smp_nofilter");

		C.r_End();
		break;
	}
}
