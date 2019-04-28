/* radare - LGPL - Copyright 2007-2017 - pancake */

#include <r_util.h>
#include <r_cons.h>

static RLine r_line_instance;
#define I r_line_instance

R_API RLine *r_line_singleton() {
	return &r_line_instance;
}

R_API RLine *r_line_new() {
	I.hist_up = NULL;
	I.hist_down = NULL;
	I.prompt = strdup ("> ");
	I.contents = NULL;
#if __WINDOWS__
	I.ansicon = r_sys_getenv ("ANSICON");
#endif
	if (!r_line_dietline_init ()) {
		eprintf ("error: r_line_dietline_init\n");
	}
	r_line_completion_init (&I.completion, 4096);
	return &I;
}

R_API void r_line_free() {
	// XXX: prompt out of the heap?
	free ((void *)I.prompt);
	I.prompt = NULL;
	r_line_hist_free ();
	r_line_completion_fini (&I.completion);
}

// handle const or dynamic prompts?
R_API void r_line_set_prompt(const char *prompt) {
	free (I.prompt);
	I.prompt = strdup (prompt);
}

// handle const or dynamic prompts?
R_API char *r_line_get_prompt() {
	return strdup (I.prompt);
}

R_API void r_line_completion_init(RLineCompletion *completion, size_t args_limit) {
	completion->run = NULL;
	completion->run_user = NULL;
	completion->args_limit = args_limit;
	r_pvector_init (&completion->args, free);
}

R_API void r_line_completion_fini(RLineCompletion *completion) {
	r_line_completion_clear (completion);
}

R_API void r_line_completion_push(RLineCompletion *completion, const char *str) {
	r_return_if_fail (completion && str);
	if (r_pvector_len (&completion->args) >= completion->args_limit) {
		return;
	}
	char *s = strdup (str);
	if (!s) {
		return;
	}
	r_pvector_push (&completion->args, (void *)s);
}

R_API void r_line_completion_set(RLineCompletion *completion, int argc, const char **argv) {
	r_return_if_fail (completion && (argc >= 0));
	r_line_completion_clear (completion);
	size_t count = R_MIN (argc, completion->args_limit);
	r_pvector_reserve (&completion->args, count);
	int i;
	for (i = 0; i < count; i++) {
		r_line_completion_push (completion, argv[i]);
	}
}

R_API void r_line_completion_clear(RLineCompletion *completion) {
	r_return_if_fail (completion);
	r_pvector_clear (&completion->args);
}

#include "dietline.c"
