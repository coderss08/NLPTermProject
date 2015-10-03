#include &lt;stdio.h&gt;#include &lt;stdlib.h&gt;

typedef struct floatList {
    float *list;
    int   size;
} *FloatList;

int floatcmp( const void *a, const void *b) {
    if  (*(float *)a &gt; *(float *)b)  return 1;
    else if (*(float *)a &lt; *(float *)b) return -1;
    else return 0;
}

float median( FloatList fl )
{
    qsort( fl-&gt;list, fl-&gt;size, sizeof(float), floatcmp);
    return 0.5 * ( fl-&gt;list[fl-&gt;size/2] + fl-&gt;list[(fl-&gt;size-1)/2]);
}

int main()
{
    static float floats1[] = { 5.1, 2.6, 6.2, 8.8, 4.6, 4.1 };
    static struct floatList flist1 = { floats1, sizeof(floats1)/sizeof(float) };

    static float floats2[] = { 5.1, 2.6, 8.8, 4.6, 4.1 };
    static struct floatList flist2 = { floats2, sizeof(floats2)/sizeof(float) };

    printf(&quot;flist1 median is %7.2f\n&quot;, median(&amp;flist1)); /* 4.85 */
    printf(&quot;flist2 median is %7.2f\n&quot;, median(&amp;flist2)); /* 4.60 */
    return 0;
}
