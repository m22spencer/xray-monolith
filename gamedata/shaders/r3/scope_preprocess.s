function normal(shader, t_base, t_second, t_detail)
	shader:begin("stub_notransform_t", "scope_preprocess")
	: zb(false, false)
	: blend(true, blend.srcalpha, blend.invsrcalpha)
	shader:dx10texture("s_image", "$user$generic0")
	shader:dx10texture("s_position", "$user$generic2")
	shader:dx10sampler("smp_base")
end