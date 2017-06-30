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
#include "keywords_search.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define is_word_char(c) (isalpha(c) || isdigit(c))

static int add_word(keywords_search_handle_t *h, const char **p)
{
	const char *start;
	keywords_search_keyword_t *word;

	start = *p;
	while (is_word_char(**p))
		(*p)++;

	if (h->num_words == h->max_num_words) {
		void *new_ptr;

		h->max_num_words += 16;
		new_ptr = realloc(h->words,
			sizeof(h->words[0]) * h->max_num_words
			);
		if (new_ptr == NULL)
			return 0;
		h->words = new_ptr;
	}
	word = &(h->words[h->num_words++]);
	word->found = 0;
	word->word =
		malloc(sizeof(word->word[0]) *
			(word->word_len = (*p)-start)+1
		);
	if (!word->word)
		return 0;
	memcpy(word->word, start, word->word_len);
	word->word[word->word_len] = '\0';

	return 1;
}

keywords_search_handle_t *keywords_search_compile(const char *pattern)
{
	int i;
	keywords_search_handle_t *ret = NULL;
	const char *char_ptr;

	ret = malloc(sizeof(*ret));
	if (!ret)
		return NULL;

	ret->num_words = 0;
	ret->max_num_words = 0;
	ret->num_found_words = 0;
	ret->words = NULL;

	char_ptr = pattern;

	while (*char_ptr) {
		if (is_word_char(*char_ptr)) {
			if (!add_word(ret, &char_ptr))
				goto cleanup_words;
		} else
			char_ptr++;
	}

	return ret;

cleanup_words:
	for (i = 0; i < ret->num_words; i++) {
		free(ret->words[i].word);
		ret->words[i].word = NULL;
	}
	free(ret->words);
	ret->words = NULL;

	free(ret);
	ret = NULL;

	return NULL;
}

static int
cmp_word(keywords_search_handle_t *handle, keywords_search_keyword_t *word,
	 const char *start, const int len)
{
	if (word->found || (word->word_len != len))
		return 0;

	if (handle->case_sensitivity)
		return !strncmp(word->word, start, len);
	else
		return !strncasecmp(word->word, start, len);
}

static int
find_word(keywords_search_handle_t *handle, const char *start, const int len)
{
	int i;
	keywords_search_keyword_t *word;

	word = handle->words;
	for (i = 0; i < handle->num_words; i++, word++)
		if (cmp_word(handle, word, start, len)) {
			word->found = 1;
			if (++handle->num_found_words == handle->num_words)
				return 1;
		}
	return 0;
}

int keywords_search_matches(keywords_search_handle_t *handle,
			    const char *string, int case_sensitivity)
{
	const char *char_ptr, *start;

	char_ptr = string;

	handle->case_sensitivity = case_sensitivity;

	if (handle->num_found_words == handle->num_words)
		return 1;

	/* Handle the NULL string gracefully. */
	if (!string)
		return 0;

	while (*char_ptr)
		if (is_word_char(*char_ptr)) {
			int len;

			start = char_ptr;
			while (is_word_char(*char_ptr))
				char_ptr++;

			len = char_ptr - start;

			if (find_word(handle, start, len))
				return 1;
		} else
			char_ptr++;

	return 0;
}

void keywords_search_reset(keywords_search_handle_t *handle)
{
	int i;

	/* Reset the state of the handle to no words found. */
	handle->num_found_words = 0;
	for (i = 0; i < handle->num_words; i++)
		handle->words[i].found = 0;
}

void keywords_search_free(keywords_search_handle_t *handle)
{
	int i;

	for (i = 0; i < handle->num_words; i++)
		free(handle->words[i].word);
	free(handle->words);
	free(handle);
}
