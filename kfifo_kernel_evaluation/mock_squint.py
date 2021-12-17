import time

DELAY_READ=1

if __name__ == "__main__":
    while True:
        lines = open("/proc/squint").readlines()
        if (lines):
            print(lines)
        time.sleep(DELAY_READ)