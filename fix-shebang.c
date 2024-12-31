/* fix-shebang.c

Copyright (C) 2024 Termux

This file is part of termux-fix-shebang.

termux-fix-shebang is free software: you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

termux-fix-shebang is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with termux-fix-shebang.  If not, see
<https://www.gnu.org/licenses/>.  */

#include <errno.h>
#include <getopt.h>
#include <libgen.h>
#include <limits.h>
#include <linux/binfmts.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ERR_LEN 256

int quiet = 0;
int dry_run = 0;

#define ARRAYELTS(arr) (sizeof (arr) / sizeof (arr)[0])

static char const *const usage_message[] =
{ "\
\n\
Replace \"standard\" shebangs with their Termux equivalent.\n\
\n\
Options:\n\
\n\
--dry-run             print info but do not replace shebangs\n\
--quiet               do not print info about replaced shebangs\n\
--help                display this help and exit\n\
--version             output version information and exit\n"
};

int check_shebang(char *filename, regex_t shebang_regex)
{
	char error_message[ERR_LEN] = {'\0'};
	char tmpfile_template[PATH_MAX] = {'\0'};
	const char *tmpdir = "TMPDIR";
	const char *env_tmpdir = getenv(tmpdir);
	if (env_tmpdir) {
		strncpy(tmpfile_template, env_tmpdir, PATH_MAX-1);
		strncat(tmpfile_template, "/%s.XXXXXX", PATH_MAX-1);
	} else {
		strncpy(tmpfile_template, TERMUX_PREFIX "/tmp/%s.XXXXXX", PATH_MAX-1);
	}

	char tmpfile[PATH_MAX];
	char shebang_line[BINPRM_BUF_SIZE];
	unsigned int max_groups = 3;
	regmatch_t matches[max_groups];
	int new_fd;

	FILE *fp = fopen(filename, "r");
	if (!fp) {
		if (snprintf(error_message, ERR_LEN-1, "fopen(\"%s\")",
			     filename) < 0)
			strncpy(error_message, "fopen()", ERR_LEN-1);
		perror(error_message);
		return 1;
	}
	fscanf(fp, "%[^\n]", shebang_line);
	int file_pos = ftell(fp);

	if (regexec(&shebang_regex, shebang_line,
		    max_groups, matches, 0))
		return 0;

	if (strncmp(shebang_line + matches[1].rm_so, "/system", 7) == 0) {
		if (!quiet)
			printf(("%s: %s: %s used as interpreter,"
				" will not change shebang\n"),
			       PACKAGE_NAME, filename,
			       shebang_line + matches[1].rm_so);
		return 0;
	}

	if (strncmp(shebang_line + matches[1].rm_so, TERMUX_PREFIX "/bin/",
		    strlen(TERMUX_PREFIX) + 5) == 0 && !quiet) {
		printf(("%s: %s: already has a termux shebang\n"),
		       PACKAGE_NAME, filename);
		return 0;
	}

	if (!quiet) {
		printf("%s: %s: rewriting %s to #!%s/bin/%s\n",
		       PACKAGE_NAME, filename, shebang_line + matches[0].rm_so,
		       TERMUX_PREFIX, shebang_line + matches[2].rm_so);
	}
	sprintf(tmpfile, tmpfile_template, basename(filename));
	new_fd = mkstemp(tmpfile);
	if (new_fd < 0) {
		if (snprintf(error_message, ERR_LEN-1, "mkstemp(\"%s\")",
			     tmpfile) < 0)
			strncpy(error_message, "mkstemp()", ERR_LEN-1);
		perror(error_message);
		return 1;
	}

	fseek(fp, 0, SEEK_END);
	int content_size = ftell(fp) - file_pos;
	fseek(fp, file_pos, SEEK_SET);

	dprintf(new_fd, "#!%s/bin/%s", TERMUX_PREFIX,
		shebang_line + matches[2].rm_so);

	char *buf = (char *)malloc(content_size + 1);
	if (!buf) {
		fprintf(stderr, "%s: buffer allocation failed\n", PACKAGE_NAME);
		return 1;
	}

	if (content_size > 1) {
		fread(buf, 1, content_size, fp);
	} else {
		strncpy(buf, "\n", 1);
	}
	dprintf(new_fd, "%s", buf);
	free(buf);

	fclose(fp);
	close(new_fd);

	if (rename(tmpfile, filename) < 0) {
		if (snprintf(error_message, ERR_LEN-1, "rename(\"%s\", \"%s\")",
			     tmpfile, filename) < 0)
			strncpy(error_message, "rename()", ERR_LEN-1);
		perror(error_message);
		return 1;
	}
	return 0;
}

int main(int argc, char **argv)
{
	int c;
	int options_index = 0;

	static struct option options[] = {
		{"dry-run", no_argument, &dry_run, 1},
		{"quiet", no_argument, &quiet, 1},
		{"help", no_argument, NULL, 'h'},
		{"version", no_argument, NULL, 'v'},
		{0, 0, 0, 0}
	};

	while (true)
	{
		c = getopt_long(argc, argv, "dqhv",
				options, &options_index);

		if (c == -1)
			break;

		switch (c) {
		case 'v':
			printf("%s %s\n", PACKAGE_NAME, PACKAGE_VERSION);
			printf(("%s\n"
				"%s comes with ABSOLUTELY NO WARRANTY.\n"
				"You may redistribute copies of %s\n"
				"under the terms of the GNU General Public License.\n"
				"For more information about these matters, "
				"see the file named COPYING.\n"),
				COPYRIGHT, PACKAGE_NAME, PACKAGE_NAME);
			return 0;
		case 'h':
			printf("Usage: %s [OPTION-OR-FILENAME]...\n", argv[0]);
			for (unsigned int i = 0; i < ARRAYELTS(usage_message); i++)
				fputs(usage_message[i], stdout);
			return 0;
		}
	}

	if (optind >= argc) {
		printf("Usage: %s [OPTION-OR-FILENAME]...\n", argv[0]);
		for (unsigned int i = 0; i < ARRAYELTS(usage_message); i++)
			fputs(usage_message[i], stdout);
		return 0;
	}

	regex_t shebang_regex;
	char error_message[ERR_LEN] = {'\0'};
	char *regex_string = "#![:space:]?(.*)/bin/(.*)";
	if (regcomp(&shebang_regex, regex_string, REG_EXTENDED)) {
		strncpy(error_message, "regcomp()", ERR_LEN-1);
		perror(error_message);
		exit(1);
	}

	char filename[PATH_MAX];
	for (int i = optind; i < argc; i++) {
		if (!realpath(argv[i], filename)) {
			if (snprintf(error_message, ERR_LEN-1,
				     "realpath(\"%s\")", argv[i]) < 0)
				strncpy(error_message, "realpath()", ERR_LEN-1);
			perror(error_message);
			return 1;
		}
		check_shebang(filename, shebang_regex);
	}
	regfree(&shebang_regex);
}
