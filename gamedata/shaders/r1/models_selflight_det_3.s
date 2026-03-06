function normal		(shader, t_base, t_second, t_detail)
	shader:begin	("model_def_hq","base_lplanes")
			: fog		(true)
	shader:sampler	("s_base")      :texture	(t_base)
	shader:sampler	("s_bump")      :texture	(t_base.."_bump")
	shader:sampler	("s_bumpX")     :texture	(t_base.."_bump#")
end
