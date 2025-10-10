#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

struct simtemp_sample {
    unsigned long long timestamp_ns;
    int temp_mC;
    int flags;
};

int main() {
    int fd = open("/dev/simtemp", O_RDONLY);
    struct simtemp_sample sample;
    
    read(fd, &sample, sizeof(sample));
    
    printf("Temp: %.1f C | Flags: 0x%x\n", 
           sample.temp_mC / 1000.0, sample.flags);
    
    close(fd);
    return 0;
}