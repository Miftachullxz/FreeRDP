
/*
#define DEFINE_UNICODE		FALSE
#define CUR_PATH_SEPARATOR_CHR	'\\'
#define CUR_PATH_SEPARATOR_STR	"\\"
#define PATH_ALLOC_COMBINE	PathAllocCombineA
*/

/**
 * FIXME: These implementations of the PathAllocCombine functions have
 * several issues:
 * - pszPathIn or pszMore may be NULL (but not both)
 * - no check if pszMore is fully qualified (if so, it must be directly
 *   copied to the output buffer without being combined with pszPathIn.
 * - if pszMore begins with a _single_ backslash it must be combined with
 *   only the root of the path pointed to by pszPathIn and there's no code
 *   to extract the root of pszPathIn.
 * - the function will crash with some short string lengths of the parameters
 */

#if DEFINE_UNICODE

HRESULT PATH_ALLOC_COMBINE(PCWSTR pszPathIn, PCWSTR pszMore, unsigned long dwFlags,
                           PWSTR* ppszPathOut)
{
	PWSTR pszPathOut;
	BOOL backslashIn;
	BOOL backslashMore;
	size_t pszMoreLength;
	size_t pszPathInLength;
	size_t pszPathOutLength;
	WLog_WARN(TAG, "has known bugs and needs fixing.");

	if (!ppszPathOut)
		return E_INVALIDARG;

	if (!pszPathIn && !pszMore)
		return E_INVALIDARG;

	if (!pszMore)
		return E_FAIL; /* valid but not implemented, see top comment */

	if (!pszPathIn)
		return E_FAIL; /* valid but not implemented, see top comment */

	pszPathInLength = _wcslen(pszPathIn);
	pszMoreLength = _wcslen(pszMore);

	/* prevent segfaults - the complete implementation below is buggy */
	if (pszPathInLength < 3)
		return E_FAIL;

	backslashIn = (pszPathIn[pszPathInLength - 1] == CUR_PATH_SEPARATOR_CHR) ? TRUE : FALSE;
	backslashMore = (pszMore[0] == CUR_PATH_SEPARATOR_CHR) ? TRUE : FALSE;

	if (backslashMore)
	{
		if ((pszPathIn[1] == ':') && (pszPathIn[2] == CUR_PATH_SEPARATOR_CHR))
		{
			const WCHAR colon[] = { ':', '\0' };
			size_t sizeOfBuffer;
			pszPathOutLength = sizeof(WCHAR) + pszMoreLength;
			sizeOfBuffer = (pszPathOutLength + 1) * sizeof(WCHAR);
			pszPathOut = (PWSTR)calloc(sizeOfBuffer, sizeof(WCHAR));

			if (!pszPathOut)
				return E_OUTOFMEMORY;

			_wcsncat(pszPathOut, &pszPathIn[0], 1);
			_wcsncat(pszPathOut, colon, ARRAYSIZE(colon));
			_wcsncat(pszPathOut, pszMore, pszMoreLength);
			*ppszPathOut = pszPathOut;
			return S_OK;
		}
	}
	else
	{
		const WCHAR sep[] = CUR_PATH_SEPARATOR_STR;
		size_t sizeOfBuffer;
		pszPathOutLength = pszPathInLength + pszMoreLength;
		sizeOfBuffer = (pszPathOutLength + 1) * 2;
		pszPathOut = (PWSTR)calloc(sizeOfBuffer, 2);

		if (!pszPathOut)
			return E_OUTOFMEMORY;

		_wcsncat(pszPathOut, pszPathIn, pszPathInLength);
		if (!backslashIn)
			_wcsncat(pszPathOut, sep, ARRAYSIZE(sep));
		_wcsncat(pszPathOut, pszMore, pszMoreLength);

		*ppszPathOut = pszPathOut;
		return S_OK;
	}

	return E_FAIL;
}

#else

HRESULT PATH_ALLOC_COMBINE(PCSTR pszPathIn, PCSTR pszMore, unsigned long dwFlags, PSTR* ppszPathOut)
{
	PSTR pszPathOut;
	BOOL backslashIn;
	BOOL backslashMore;
	int pszMoreLength;
	int pszPathInLength;
	int pszPathOutLength;
	WLog_WARN(TAG, "has known bugs and needs fixing.");

	if (!ppszPathOut)
		return E_INVALIDARG;

	if (!pszPathIn && !pszMore)
		return E_INVALIDARG;

	if (!pszMore)
		return E_FAIL; /* valid but not implemented, see top comment */

	if (!pszPathIn)
		return E_FAIL; /* valid but not implemented, see top comment */

	pszPathInLength = lstrlenA(pszPathIn);
	pszMoreLength = lstrlenA(pszMore);

	/* prevent segfaults - the complete implementation below is buggy */
	if (pszPathInLength < 3)
		return E_FAIL;

	backslashIn = (pszPathIn[pszPathInLength - 1] == CUR_PATH_SEPARATOR_CHR) ? TRUE : FALSE;
	backslashMore = (pszMore[0] == CUR_PATH_SEPARATOR_CHR) ? TRUE : FALSE;

	if (backslashMore)
	{
		if ((pszPathIn[1] == ':') && (pszPathIn[2] == CUR_PATH_SEPARATOR_CHR))
		{
			size_t sizeOfBuffer;
			pszPathOutLength = 2 + pszMoreLength;
			sizeOfBuffer = (pszPathOutLength + 1) * 2;
			pszPathOut = (PSTR)calloc(sizeOfBuffer, 2);

			if (!pszPathOut)
				return E_OUTOFMEMORY;

			sprintf_s(pszPathOut, sizeOfBuffer, "%c:%s", pszPathIn[0], pszMore);
			*ppszPathOut = pszPathOut;
			return S_OK;
		}
	}
	else
	{
		size_t sizeOfBuffer;
		pszPathOutLength = pszPathInLength + pszMoreLength;
		sizeOfBuffer = (pszPathOutLength + 1) * 2;
		pszPathOut = (PSTR)calloc(sizeOfBuffer, 2);

		if (!pszPathOut)
			return E_OUTOFMEMORY;

		if (backslashIn)
			sprintf_s(pszPathOut, sizeOfBuffer, "%s%s", pszPathIn, pszMore);
		else
			sprintf_s(pszPathOut, sizeOfBuffer, "%s" CUR_PATH_SEPARATOR_STR "%s", pszPathIn,
			          pszMore);

		*ppszPathOut = pszPathOut;
		return S_OK;
	}

	return E_FAIL;
}

#endif

/*
#undef DEFINE_UNICODE
#undef CUR_PATH_SEPARATOR_CHR
#undef CUR_PATH_SEPARATOR_STR
#undef PATH_ALLOC_COMBINE
*/
