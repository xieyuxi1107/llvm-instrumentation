import time

DELAY_READ=0.01

if __name__ == "__main__":
    total_read_so_far = 0
    while True:
        lines = open("/proc/squint").readlines()
        if (lines):
            num_flushes = len([line for line in lines if line == '@ flush\n'])
            total_read_so_far += num_flushes
            print(f"total_read_so_far: {total_read_so_far}")

        time.sleep(DELAY_READ)