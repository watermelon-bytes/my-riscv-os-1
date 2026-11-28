local targets = {
	run = "run_in_qemu",
	test = "custom_test_target_name",
	debug = "debug_target",
}

vim.api.nvim_create_user_command("Make", function(opts)
	local target = targets[opts.args]

	if not target then
		error("Unknown target: " .. opts.args)
	end

	vim.opt.makeprg = "cmake --build build --target " .. target
	vim.cmd("make")
end, { nargs = 1 })
