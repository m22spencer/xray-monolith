function normal(shader, t_base, t_second, t_detail)
	shader:begin("stub_notransform_t", "scope_debug")
	: zb(false, false)
	: blend(true, blend.srcalpha, blend.invsrcalpha)
	shader:dx10texture("vp2", "$user$viewport2")
	shader:dx10sampler("smp_base")
end