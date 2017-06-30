/*
 * Keywords' Search Micro-Library.
 *
 * Copyrighted by Shlomi Fish, 2007.
 *
 * Licensed under the MIT/Expat License:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 */
#ifndef KEYWORDS_SEARCH_H
#define KEYWORDS_SEARCH_H

typedef struct {
	int found;
	char *word;
	int word_len;
} keywords_search_keyword_t;

typedef struct {
	int num_words;
	int max_num_words;
	int num_found_words;
	keywords_search_keyword_t *words;
	/* The case sensitivity flag - valid only for a search session. */
	int case_sensitivity;
} keywords_search_handle_t;

keywords_search_handle_t *keywords_search_compile(const char *pattern);
int keywords_search_matches(keywords_search_handle_t *handle,
			    const char *string, int case_sensitivity);
void keywords_search_reset(keywords_search_handle_t *handle);
void keywords_search_free(keywords_search_handle_t *handle);

#endif /* KEYWORDS_SEARCH_H */

