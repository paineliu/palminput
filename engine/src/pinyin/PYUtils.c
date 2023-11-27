#include "PYUtils.h"

PY_UINT32 PY_wcslen(PY_PCWSTR pSrc)
{
    PY_PCWSTR pTmp = pSrc;

    while (*pTmp++) 
    {
        ;
    }

   return (PY_UINT32) (pTmp - pSrc - 1);
}

PY_PSTR PY_strcpy(PY_PSTR pDst, PY_PCSTR pSrc)
{
    PY_PSTR pRet = pDst;

    while ((*pDst++ = *pSrc++) != 0)
    {
        ;
    }

    return pRet;
}

PY_PWSTR PY_wcscpy(PY_PWSTR pDst, PY_PCWSTR pSrc)
{
    PY_PWSTR pRet = pDst;
    
    while ((*pDst++ = *pSrc++) != 0)
    {
         ;
    }

    return pRet;
}

PY_PWSTR PY_swncpy(PY_PWSTR pDst, PY_PCSTR pSrc, PY_UINT32 nCount)
{
    PY_PWSTR pStart = pDst;

    while (nCount && ((*pDst++ = *pSrc++) != 0))    /* copy string */
    {
        nCount--;
    }

    if (nCount)                              /* pad out with zeroes */
    {
        while (--nCount)
            *pDst++ = 0;
    }

    return (pStart);

}

PY_PSTR PY_strncpy(PY_PSTR pDst, PY_PCSTR pSrc, PY_UINT32 nCount)
{
    PY_PSTR pStart = pDst;

    while (nCount && ((*pDst++ = *pSrc++) != 0))    /* copy string */
    {
        nCount--;
    }

    if (nCount)                              /* pad out with zeroes */
    {
        while (--nCount)
            *pDst++ = 0;
    }

    return (pStart);

}

PY_PWSTR PY_wcsncpy(PY_PWSTR pDst, PY_PCWSTR pSrc, PY_UINT32 nCount)
{
    PY_PWSTR pStart = pDst;

    while (nCount && ((*pDst++ = *pSrc++) != 0))    /* copy string */
    {
        nCount--;
    }

    if (nCount)                              /* pad out with zeroes */
    {
        while (--nCount)
            *pDst++ = 0;
    }

    return (pStart);
}

PY_PWSTR PY_wcscat(PY_PWSTR pDst, PY_PCWSTR pSrc)
{
    PY_PWSTR pTmp = pDst;

    while (*pTmp)
    {
            pTmp++;
    }
    
    while ((*pTmp++ = *pSrc++) != 0)
    {
        ;
    }
    
    return (pDst);
}

PY_INT32 PY_strncmp(PY_PCSTR pSrc, PY_PCSTR pDst, PY_UINT32 nCount)
{
    if (nCount == 0)
    {
        return(0);
    }

    while (--nCount && *pSrc && *pSrc == *pDst)
    {
        pSrc++;
        pDst++;
    }

    return ((PY_INT32)(*pSrc - *pDst));
}

PY_INT32 PY_wcsncmp(PY_PCWSTR pSrc, PY_PCWSTR pDst, PY_UINT32 nCount)
{
    if (nCount == 0)
    {
        return(0);
    }

    while (--nCount && *pSrc && *pSrc == *pDst)
    {
        pSrc++;
        pDst++;
    }

    return ((PY_INT32) (*pSrc - *pDst));
}

PY_INT32 PY_strcmp(PY_PCSTR pWcs1, PY_PCSTR pWcs2)
{
    if (PY_NULL != pWcs1 && PY_NULL != pWcs2)
    {
        while (*pWcs1 == *pWcs2 && 0 != *pWcs1)
        {
            pWcs1++;
            pWcs2++;
        }
        
        return ((PY_INT32)*pWcs1 - (PY_INT32)*pWcs2);
    }
    
    return 0;
}

PY_INT32 PY_wcsstrcmp(PY_PCWSTR pSrc, PY_PCSTR pDst)
{
    PY_INT32 pRet = 0;

    while (((pRet = (int)(*pSrc - *pDst)) == 0) && *pDst)
    {
        ++pSrc, ++pDst;
    }

    if (pRet < 0)
    {
        pRet = -1;
    }
    else if (pRet > 0)
    {
        pRet = 1;
    }

    return (pRet);
}


PY_INT32 PY_wcscmp(PY_PCWSTR pSrc, PY_PCWSTR pDst)
{
    PY_INT32 pRet = 0 ;

    while (((pRet = (int) (*pSrc - *pDst)) == 0) && *pDst)
    {
        ++pSrc, ++pDst;
    }
        
    if (pRet < 0)
    {
        pRet = -1 ;
    }
    else if (pRet > 0)
    {
       pRet = 1 ;
    }

    return (pRet);
}

/*
PY_PVOID PY_memzero(PY_PVOID pDst, PY_UINT32 nCount)
{
    void *pStart = pDst;
    
    while (nCount--) 
    {
        *(char *)pDst = 0;
        pDst = (char *) pDst + 1;
    }
    
    return (pStart);
}
*/

PY_PVOID PY_memcpy(PY_PVOID pDst, PY_PCVOID pSrc, PY_UINT32 nCount)
{
    void * pRet = pDst;

    while (nCount--) 
    {
        *(char *)pDst = *(char *)pSrc;
        pDst = (char *)pDst + 1;
        pSrc = (char *)pSrc + 1;
    }

    return (pRet);
}

PY_PVOID PY_memmove(PY_PVOID pDst, PY_PCVOID pSrc, PY_UINT32 nCount)
{
    void * pRet = pDst;

    if (pDst <= pSrc || (char *)pDst >= ((char *)pSrc + nCount)) 
    {
        while (nCount--) 
        {
            *(char *)pDst = *(char *)pSrc;
            pDst = (char *)pDst + 1;
            pSrc = (char *)pSrc + 1;
        }
    }
    else 
    {
        pDst = (char *)pDst + nCount - 1;
        pSrc = (char *)pSrc + nCount - 1;

        while (nCount--) 
        {
                *(char *)pDst = *(char *)pSrc;
                pDst = (char *)pDst - 1;
                pSrc = (char *)pSrc - 1;
        }
    }

    return (pRet);
}


PY_INT32 PY_memcmp(PY_PVOID buffer1, PY_PCVOID buffer2, PY_UINT32 count)
{
    if (!count)
        return(0);

    while ( --count && *(char *)buffer1 == *(char *)buffer2) 
    {

        buffer1 = (char *)buffer1 + 1;
        buffer2 = (char *)buffer2 + 1;
    }

    return( *((unsigned char *)buffer1) - *((unsigned char *)buffer2) );
}

     
PY_PVOID PY_memset(PY_PVOID pDst, PY_INT32 nVal, PY_UINT32 nCount)
{
    PY_PVOID pStart = pDst;

    while (nCount--) 
    {
        *(char *) pDst = (char) nVal;
        pDst = (char *) pDst + 1;
    }

    return (pStart);
}

PY_BOOL PY_isletter(PY_PWSTR pStr, PY_UINT32 nLen)
{
    PY_UINT32 i;

    for (i=0; i<nLen; i++)
    {
        if (!PY_ISLETTER(pStr[i]))
        {
            return PY_FALSE;
        }
    }
            
    return PY_TRUE;
}

PY_BOOL PY_existhanletter(PY_PWSTR pStr, PY_UINT32 nLen)
{
    PY_UINT32 i;

    for (i=0; i<nLen; i++)
    {
        if (PY_ISHANLETTER(pStr[i]))
        {
            return PY_TRUE;
        }
    }
            
    return PY_FALSE;
}

PY_BOOL PY_ishanletter(PY_PWSTR pStr, PY_UINT32 nLen)
{
    PY_UINT32 i;

    for (i=0; i<nLen; i++)
    {
        if (!PY_ISHANLETTER(pStr[i]))
        {
            return PY_FALSE;
        }
    }
            
    return PY_TRUE;
}

PY_PWSTR PY_tolower(PY_PWSTR pStr)
{
    PY_PWSTR p = pStr;

    while (*p != '\0')
    {
        if(*p >= L'A' && *p <= L'Z')
            *p = (*p) + 0x20;

        p++;
    }

    return pStr;
}

PY_PSTR PY_stoupper(PY_PSTR pStr)

{
    PY_PSTR p = pStr;

    while (*p != '\0')
    {

        if (*p >= 'a' && *p <= 'z')
            *p = (*p) - 0x20;

        p++;
    }

    return pStr;
}

PY_PWSTR PY_toupper(PY_PWSTR pStr)

{
    PY_PWSTR p = pStr;

    while (*p != '\0')
    {

        if(*p >= L'a' && *p <= L'z')
            *p = (*p) - 0x20;

        p++;
    }

    return pStr;
}

/************************************************************************/
/* calculate log value of x based on 10                                 */
/* @pre x has to be greater than 1e-8, else return 0xfffffff            */
/************************************************************************/

double PY_log10(double x)
{
	double logx;
	PY_INT32 n=0, i;
	PY_UINT32 k=1;

	if(x<=1e-8)
	{
		return 0xffffffff;
	}

	while(x<1)
	{
		x *= 10;
		n--;
	}

	while(x>=10)
	{
		x /= 10;
		n++;
	}
	logx = n;

	for(i=1; i<=31; i++)
	{
		k *= 2;
		if(x*x >= 10)
		{
			x = x*x/10;
			logx += 1.0/k;
		}
		else
		{
			x = x*x;
		}
	}
	return logx;
}

#define SWAP(a, b, size)                              \
do                                          \
{                                          \
PY_UINT32 __size = (size);                          \
char *__a = (a), *__b = (b);                      \
do                                      \
{                                      \
char __tmp = *__a;                              \
*__a++ = *__b;                              \
*__b++ = __tmp;                              \
} while (--__size > 0);                              \
} while (size != size)

/* Discontinue quicksort algorithm when partition gets below this size.
 This particular magic number was chosen to work best on a Sun 4/260. */
#define MAX_THRESH 4

/* Stack node declarations used to store unfulfilled partition obligations. */
typedef struct
{
    char *lo;
    char *hi;
} stack_node;

/* The next 4 #defines implement a very fast in-line stack abstraction. */
/* The stack needs log (total_elements) entries (we could even subtract
 log(MAX_THRESH)).  Since total_elements has type size_t, we get as
 upper bound for log (total_elements):
 bits per byte (CHAR_BIT) * sizeof(size_t).  */
#define CHAR_BIT  8

#define STACK_SIZE    (CHAR_BIT * sizeof(PY_UINT32))
#define PUSH(low, high)    ((void) ((top->lo = (low)), (top->hi = (high)), ++top))
#define    POP(low, high)    ((void) (--top, (low = top->lo), (high = top->hi)))
#define    STACK_NOT_EMPTY    (stack < top)


/* Order size using quicksort.  This implementation incorporates
 four optimizations discussed in Sedgewick:
 
 1. Non-recursive, using an explicit stack of pointer that store the
 next array partition to sort.  To save time, this maximum amount
 of space required to store an array of SIZE_MAX is allocated on the
 stack.  Assuming a 32-bit (64 bit) integer for size_t, this needs
 only 32 * sizeof(stack_node) == 256 bytes (for 64 bit: 1024 bytes).
 Pretty cheap, actually.
 
 2. Chose the pivot element using a median-of-three decision tree.
 This reduces the probability of selecting a bad pivot value and
 eliminates certain extraneous comparisons.
 
 3. Only quicksorts TOTAL_ELEMS / MAX_THRESH partitions, leaving
 insertion sort to order the MAX_THRESH items within each partition.
 This is a big win, since insertion sort is faster for small, mostly
 sorted array segments.
 
 4. The larger of the two sub-partitions is always pushed onto the
 stack first, with the algorithm then concentrating on the
 smaller partition.  This *guarantees* no more than log (total_elems)
 stack size is needed (actually O(1) in this case)!  */

void PY_quicksort(void *const pbase, PY_UINT32 total_elems, PY_UINT32 size,
                   PY_compare  cmp, void *arg)
{
    char *base_ptr = (char *) pbase;
    
    const PY_UINT32 max_thresh = MAX_THRESH * size;
    
    if (total_elems == 0)
    /* Avoid lossage with unsigned arithmetic below.  */
        return;
    
    if (total_elems > MAX_THRESH)
    {
        char *lo = base_ptr;
        char *hi = &lo[size * (total_elems - 1)];
        stack_node stack[STACK_SIZE];
        stack_node *top = stack;
        
        PUSH (0, 0);
        
        while (STACK_NOT_EMPTY)
        {
            char *left_ptr;
            char *right_ptr;
            
            /* Select median value from among LO, MID, and HI. Rearrange
             LO and HI so the three values are sorted. This lowers the
             probability of picking a pathological pivot value and
             skips a comparison for both the LEFT_PTR and RIGHT_PTR in
             the while loops. */
            
            char *mid = lo + size * ((hi - lo) / size >> 1);
            
            if ((*cmp) ((void *) mid, (void *) lo, arg) < 0)
                SWAP (mid, lo, size);
            if ((*cmp) ((void *) hi, (void *) mid, arg) < 0)
                SWAP (mid, hi, size);
            else
                goto jump_over;
            if ((*cmp) ((void *) mid, (void *) lo, arg) < 0)
                SWAP (mid, lo, size);
        jump_over:;
            
            left_ptr  = lo + size;
            right_ptr = hi - size;
            
            /* Here's the famous ``collapse the walls'' section of quicksort.
             Gotta like those tight inner loops!  They are the main reason
             that this algorithm runs much faster than others. */
            do
            {
                while ((*cmp) ((void *) left_ptr, (void *) mid, arg) < 0)
                    left_ptr += size;
                
                while ((*cmp) ((void *) mid, (void *) right_ptr, arg) < 0)
                    right_ptr -= size;
                
                if (left_ptr < right_ptr)
                {
                    SWAP (left_ptr, right_ptr, size);
                    if (mid == left_ptr)
                        mid = right_ptr;
                    else if (mid == right_ptr)
                        mid = left_ptr;
                    left_ptr += size;
                    right_ptr -= size;
                }
                else if (left_ptr == right_ptr)
                {
                    left_ptr += size;
                    right_ptr -= size;
                    break;
                }
            }
            while (left_ptr <= right_ptr);
            
            /* Set up pointers for next iteration.  First determine whether
             left and right partitions are below the threshold size.  If so,
             ignore one or both.  Otherwise, push the larger partition's
             bounds on the stack and continue sorting the smaller one. */
            
            if ((PY_UINT32) (right_ptr - lo) <= max_thresh)
            {
                if ((PY_UINT32) (hi - left_ptr) <= max_thresh)
                /* Ignore both small partitions. */
                    POP (lo, hi);
                else
                /* Ignore small left partition. */
                    lo = left_ptr;
            }
            else if ((PY_UINT32) (hi - left_ptr) <= max_thresh)
            /* Ignore small right partition. */
                hi = right_ptr;
            else if ((right_ptr - lo) > (hi - left_ptr))
            {
                /* Push larger left partition indices. */
                PUSH (lo, right_ptr);
                lo = left_ptr;
            }
            else
            {
                /* Push larger right partition indices. */
                PUSH (left_ptr, hi);
                hi = right_ptr;
            }
        }
    }
    
    /* Once the BASE_PTR array is partially sorted by quicksort the rest
     is completely sorted using insertion sort, since this is efficient
     for partitions below MAX_THRESH size. BASE_PTR points to the beginning
     of the array to sort, and END_PTR points at the very last element in
     the array (*not* one beyond it!). */
    
#define min(x, y) ((x) < (y) ? (x) : (y))
    
    {
        char *const end_ptr = &base_ptr[size * (total_elems - 1)];
        char *tmp_ptr = base_ptr;
        char *thresh = min(end_ptr, base_ptr + max_thresh);
        char *run_ptr;
        
        /* Find smallest element in first threshold and place it at the
         array's beginning.  This is the smallest array element,
         and the operation speeds up insertion sort's inner loop. */
        
        for (run_ptr = tmp_ptr + size; run_ptr <= thresh; run_ptr += size)
            if ((*cmp) ((void *) run_ptr, (void *) tmp_ptr, arg) < 0)
                tmp_ptr = run_ptr;
        
        if (tmp_ptr != base_ptr)
            SWAP (tmp_ptr, base_ptr, size);
        
        /* Insertion sort, running from left-hand-side up to right-hand-side.  */
        
        run_ptr = base_ptr + size;
        while ((run_ptr += size) <= end_ptr)
        {
            tmp_ptr = run_ptr - size;
            while ((*cmp) ((void *) run_ptr, (void *) tmp_ptr, arg) < 0)
                tmp_ptr -= size;
            
            tmp_ptr += size;
            if (tmp_ptr != run_ptr)
            {
                char *trav;
                
                trav = run_ptr + size;
                while (--trav >= run_ptr)
                {
                    char c = *trav;
                    char *hi, *lo;
                    
                    for (hi = lo = trav; (lo -= size) >= tmp_ptr; hi = lo)
                        *hi = *lo;
                    *hi = c;
                }
            }
        }
    }
}

PY_PVOID PY_binsearch(const PY_VOID *key, const PY_VOID *base, PY_UINT32 num, PY_UINT32 width, PY_compare cmp, void *arg)
{
    PY_UINT32 l, u, idx;
    const void *p;
    PY_INT32 comparison;
    
    l = 0;
    u = num;
    
    while (l < u)
    {
        idx = (l + u) / 2;
        p = (void *) (((const char *) base) + (idx * width));
        comparison = (*cmp) (key, p, arg);
        if (comparison < 0)
            u = idx;
        else if (comparison > 0)
            l = idx + 1;
        else
            return (void *) p;
    }
    
    return (PY_VOID *) (0);
}

PY_PVOID PY_binsearch_nocmp(const PY_VOID *key, const PY_VOID *base, PY_UINT32 num, PY_UINT32 width)
{
    PY_UINT32 l, u, idx;
    const void *p;
    PY_INT32 comparison;
    
    l = 0;
    u = num;
    
    while (l < u)
    {
        idx = (l + u) / 2;
        p = (void *) (((const char *) base) + (idx * width));
        comparison = *(PY_UINT32 *)key - *(PY_UINT32 *)p;
        if (comparison < 0)
            u = idx;
        else if (comparison > 0)
            l = idx + 1;
        else
            return (void *) p;
    }
    
    return (PY_VOID *) (0);
}

PY_UINT32 PY_binsearchmin(const PY_VOID *key, const PY_VOID *pbase, PY_UINT32 nnum, PY_UINT32 width, PY_compare cmp, void *arg)
{
    PY_UINT32 l, u, idx;
    const void *p;
    PY_INT32 r_comp_prev;
    PY_INT32 r_comp_current;
    
    l = 0;
    u = nnum;
    
    while (l < u)
    {
        idx = (l + u) / 2;
        
        p = (void *) (((const char *) pbase) + (idx * width));
        r_comp_current = (*cmp) (key, p, arg);
        
        if (idx > 0)
        {
            p = (void *) (((const char *) pbase) + ((idx - 1) * width));
            r_comp_prev = (*cmp) (key, p, arg);
        }
        else
        {
            r_comp_prev = 1;
        }
        
        if (r_comp_current <= 0)
        {
            if (r_comp_prev > 0)
            {
                return idx;
            }
            else
            {
                u = idx;
            }
        }
        else
        {
            l = idx + 1;
        }
    }
    
    return (l + u) / 2;
}

PY_UINT32 PY_binsearchmin_nocmp(const PY_VOID *key, const PY_VOID *pbase, PY_UINT32 nnum, PY_UINT32 width)
{
    PY_UINT32 l, u, idx;
    const void *p;
    PY_INT32 r_comp_prev;
    PY_INT32 r_comp_current;
    
    l = 0;
    u = nnum;
    
    while (l < u)
    {
        idx = (l + u) / 2;
        
        p = (void *) (((const char *) pbase) + (idx * width));
        r_comp_current = *(PY_UINT32 *)key - *(PY_UINT32 *)p;
        
        if (idx > 0)
        {
            p = (void *) (((const char *) pbase) + ((idx - 1) * width));
            r_comp_prev = *(PY_UINT32 *)key - *(PY_UINT32 *)p;
        }
        else
        {
            r_comp_prev = 1;
        }
        
        if (r_comp_current <= 0)
        {
            if (r_comp_prev > 0)
            {
                return idx;
            }
            else
            {
                u = idx;
            }
        }
        else
        {
            l = idx + 1;
        }
    }
    
    return (l + u) / 2;
}

PY_UINT32 PY_binsearchmax(const PY_VOID *key, const PY_VOID *pbase, PY_UINT32 nnum, PY_UINT32 width, PY_compare cmp, void *arg)
{
    PY_UINT32 l, u, idx;
    const void *p;
    PY_INT32 r_comp_current;
    PY_INT32 r_comp_next;
    
    l = 0;
    u = nnum;
    
    while (l < u)
    {
        idx = (l + u) / 2;
        
        p = (void *) (((const char *) pbase) + (idx * width));
        r_comp_current = (*cmp) (key, p, arg);
        
        if (idx < nnum - 1)
        {
            p = (void *) (((const char *) pbase) + ((idx + 1) * width));
            r_comp_next = (*cmp) (key, p, arg);
        }
        else
        {
            r_comp_next = -1;
        }
        
        if (r_comp_current >= 0)
        {
            if (r_comp_next < 0)
            {
                return idx + 1;
            }
            else
            {
                l = idx + 1;
            }
        }
        else
        {
            u = idx;
        }
    }
    
    return (l + u) / 2;
}

PY_INT32 PY_utf16_to_utf8_one(PY_UINT32 nUniCode, PY_CHAR *pUTF8Char, PY_INT32 nMaxSize)
{
    if (nUniCode <= 0x0000007F)
    {
        // U-00000000 - U-0000007F:  0xxxxxxx
        if (nMaxSize >= 1)
        {
            *pUTF8Char = (nUniCode & 0x7F);
        }
        return 1;
    }
    else if (nUniCode >= 0x00000080 && nUniCode <= 0x000007FF)
    {
        // U-00000080 - U-000007FF:  110xxxxx 10xxxxxx
        if (nMaxSize >= 2)
        {
            *(pUTF8Char + 1) = (nUniCode & 0x3F) | 0x80;
            *pUTF8Char = ((nUniCode >> 6) & 0x1F) | 0xC0;
        }
        return 2;
    }
    else if (nUniCode >= 0x00000800 && nUniCode <= 0x0000FFFF)
    {
        // U-00000800 - U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx
        if (nMaxSize >= 3)
        {
            *(pUTF8Char + 2) = (nUniCode & 0x3F) | 0x80;
            *(pUTF8Char + 1) = ((nUniCode >> 6) & 0x3F) | 0x80;
            *pUTF8Char = ((nUniCode >> 12) & 0x0F) | 0xE0;
        }
        return 3;
    }
    else if (nUniCode >= 0x00010000 && nUniCode <= 0x001FFFFF)
    {
        // U-00010000 - U-001FFFFF:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        if (nMaxSize >= 4)
        {
            *(pUTF8Char + 3) = (nUniCode & 0x3F) | 0x80;
            *(pUTF8Char + 2) = ((nUniCode >> 6) & 0x3F) | 0x80;
            *(pUTF8Char + 1) = ((nUniCode >> 12) & 0x3F) | 0x80;
            *pUTF8Char = ((nUniCode >> 18) & 0x07) | 0xF0;
        }
        return 4;
    }
    else if (nUniCode >= 0x00200000 && nUniCode <= 0x03FFFFFF)
    {
        // U-00200000 - U-03FFFFFF:  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        if (nMaxSize >= 5)
        {
            *(pUTF8Char + 4) = (nUniCode & 0x3F) | 0x80;
            *(pUTF8Char + 3) = ((nUniCode >> 6) & 0x3F) | 0x80;
            *(pUTF8Char + 2) = ((nUniCode >> 12) & 0x3F) | 0x80;
            *(pUTF8Char + 1) = ((nUniCode >> 18) & 0x3F) | 0x80;
            *pUTF8Char = ((nUniCode >> 24) & 0x03) | 0xF8;
        }
        return 5;
    }
    else if (nUniCode >= 0x04000000 && nUniCode <= 0x7FFFFFFF)
    {
        // U-04000000 - U-7FFFFFFF:  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        if (nMaxSize >= 6)
        {
            *(pUTF8Char + 5) = (nUniCode & 0x3F) | 0x80;
            *(pUTF8Char + 4) = ((nUniCode >> 6) & 0x3F) | 0x80;
            *(pUTF8Char + 3) = ((nUniCode >> 12) & 0x3F) | 0x80;
            *(pUTF8Char + 2) = ((nUniCode >> 18) & 0x3F) | 0x80;
            *(pUTF8Char + 1) = ((nUniCode >> 24) & 0x3F) | 0x80;
            *pUTF8Char = ((nUniCode >> 30) & 0x01) | 0xFC;
        }
        return 6;
    }

    return 0;
}

PY_INT32 PY_utf16_to_utf8(PY_UINT16 *utf16, PY_INT32 utf16_len, PY_CHAR *utf8, PY_INT32 utf8_max_len)
{
    PY_INT32 utf8_use_len = 0;
    PY_INT32 i;
    PY_INT32 len;

    for (i=0; i<utf16_len; i++)
    {
        len = PY_utf16_to_utf8_one(utf16[i], &utf8[utf8_use_len], utf8_max_len - utf8_use_len);
        utf8_use_len += len;
    }

    if (utf8_max_len > utf8_use_len)
    {
        utf8[utf8_use_len] = 0;
    }

    return utf8_use_len;
}

int enc_get_utf8_size(const char *pInput)
{
    unsigned char c = *((unsigned char *)pInput);
//    printf("---c=%c---\n", c);
    if(c< 0x80) return 1;                // 0xxxxxxx 返回0
    if(c>=0x80 && c<0xC0) return -1;     // 10xxxxxx 返回-1
    if(c>=0xC0 && c<0xE0) return 2;      // 110xxxxx 返回2
    if(c>=0xE0 && c<0xF0) return 3;      // 1110xxxx 返回3
    if(c>=0xF0 && c<0xF8) return 4;      // 11110xxx 返回4
    if(c>=0xF8 && c<0xFC) return 5;      // 111110xx 返回5
    if(c>=0xFC) return 6;                // 1111110x 返回6
    return 1;
}


int enc_utf8_to_unicode_one(const char* pInput, unsigned long *Unic)
{
    //assert(pInput != NULL && Unic != NULL);

    // b1 表示UTF-8编码的pInput中的高字节, b2 表示次高字节, ...
    char b1, b2, b3, b4, b5, b6;

    *Unic = 0x0; // 把 *Unic 初始化为全零
    int utfbytes = enc_get_utf8_size(pInput);
    unsigned char *pOutput = (unsigned char *) Unic;
    //printf("----utfbytes=%d\n", utfbytes);
    switch ( utfbytes )
    {
        case 1:
            *pOutput     = *pInput;
            break;

        case 2:
            b1 = *pInput;
            b2 = *(pInput + 1);
            if ( (b2 & 0xE0) != 0x80 )
                return 0;
            *pOutput     = (b1 << 6) + (b2 & 0x3F);
            *(pOutput+1) = (b1 >> 2) & 0x07;
            break;

        case 3:
            b1 = *pInput;
            b2 = *(pInput + 1);
            b3 = *(pInput + 2);
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80) )
                return 0;
            *pOutput     = (b2 << 6) + (b3 & 0x3F);
            *(pOutput+1) = (b1 << 4) + ((b2 >> 2) & 0x0F);
            break;

        case 4:
            b1 = *pInput;
            b2 = *(pInput + 1);
            b3 = *(pInput + 2);
            b4 = *(pInput + 3);
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)
                 || ((b4 & 0xC0) != 0x80) )
                return 0;
            *pOutput     = (b3 << 6) + (b4 & 0x3F);
            *(pOutput+1) = (b2 << 4) + ((b3 >> 2) & 0x0F);
            *(pOutput+2) = ((b1 << 2) & 0x1C)  + ((b2 >> 4) & 0x03);
            break;

        case 5:
            b1 = *pInput;
            b2 = *(pInput + 1);
            b3 = *(pInput + 2);
            b4 = *(pInput + 3);
            b5 = *(pInput + 4);
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)
                 || ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80) )
                return 0;
            *pOutput     = (b4 << 6) + (b5 & 0x3F);
            *(pOutput+1) = (b3 << 4) + ((b4 >> 2) & 0x0F);
            *(pOutput+2) = (b2 << 2) + ((b3 >> 4) & 0x03);
            *(pOutput+3) = (b1 << 6);
            break;

        case 6:
            b1 = *pInput;
            b2 = *(pInput + 1);
            b3 = *(pInput + 2);
            b4 = *(pInput + 3);
            b5 = *(pInput + 4);
            b6 = *(pInput + 5);
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)
                 || ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80)
                 || ((b6 & 0xC0) != 0x80) )
                return 0;
            *pOutput     = (b5 << 6) + (b6 & 0x3F);
            *(pOutput+1) = (b5 << 4) + ((b6 >> 2) & 0x0F);
            *(pOutput+2) = (b3 << 2) + ((b4 >> 4) & 0x03);
            *(pOutput+3) = ((b1 << 6) & 0x40) + (b2 & 0x3F);
            break;

        default:
            return 0;
            break;
    }

    return utfbytes;
}

int enc_utf8_to_utf16_one(const char* pInput, unsigned short *Unic)
{
    //assert(pInput != NULL && Unic != NULL);
    if (Unic == NULL) {
        int utfbytes = enc_get_utf8_size(pInput);
        if (utfbytes == -1) {
            return 1;
        }
        return utfbytes;
    }

    // b1 表示UTF-8编码的pInput中的高字节, b2 表示次高字节, ...
    char b1, b2, b3;

    *Unic = 0x0; // 把 *Unic 初始化为全零
    int utfbytes = enc_get_utf8_size(pInput);
    unsigned char *pOutput = (unsigned char *) Unic;
    //printf("----utfbytes=%d\n", utfbytes);
    switch ( utfbytes )
    {
        case -1:
            return 1;

        case 1:
            *pOutput     = *pInput;
            break;

        case 2:
            b1 = *pInput;
            b2 = *(pInput + 1);
            if ( (b2 & 0xE0) != 0x80 )
                return 0;
            *pOutput     = (b1 << 6) + (b2 & 0x3F);
            *(pOutput+1) = (b1 >> 2) & 0x07;
            break;

        case 3:
            b1 = *pInput;
            b2 = *(pInput + 1);
            b3 = *(pInput + 2);
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80) )
                return 0;
            *pOutput     = (b2 << 6) + (b3 & 0x3F);
            *(pOutput+1) = (b1 << 4) + ((b2 >> 2) & 0x0F);
            break;

    }

    return utfbytes;
}

PY_INT32 PY_utf8_to_utf16(PY_PCSTR utf8, PY_INT32 utf8_len, PY_UINT16 *utf16, PY_INT32 utf16_max_len)
{
    int nUtf8Begin = 0;
    int nUtf16Begin = 0;
    int nUtf8ItemLen = 0;

    nUtf8ItemLen = enc_utf8_to_utf16_one(&utf8[nUtf8Begin], NULL);
    while (utf8_len >= nUtf8Begin + nUtf8ItemLen)
    {
        enc_utf8_to_utf16_one(&utf8[nUtf8Begin], &utf16[nUtf16Begin]);
        if (utf16[nUtf16Begin] != 0)
        {
            nUtf16Begin ++;
        }
        nUtf8Begin += nUtf8ItemLen;
        nUtf8ItemLen = enc_utf8_to_utf16_one(&utf8[nUtf8Begin], NULL);

    }

    return nUtf16Begin;
}
