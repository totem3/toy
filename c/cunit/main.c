void sort(int array[], int num)
{
    int i;
    int j;
    int val;
    for (i = 0; i < (num - 1); i++) {
        for (j = (num - 1); j > i; j --) {
            if (array[j-1] > array[j]) {
                val = array[j];
                array[j] = array[i];
                array[i] = val;
            }
        }
    }
}

