import argparse

def parse_time_info(fp):
    numbers = []
    for line in fp:
        if (line.strip() != '\n'):
            time_str = line.split('m')[1]
            time_str = time_str.replace('s', '')
            numbers.append(float(time_str))
    return numbers

if __name__ == "__main__":
    # parser = argparse.ArgumentParser(description='Input  name.')
    # parser.add_argument('input', type=str,
    #                     help='input name')
    # input_file = parser.parse_args().input

    compiler_time = open("test_compile_time_user.txt", "r")
    numbers = parse_time_info(compiler_time)
    avg_runtime = sum(numbers) / len(numbers)
    print(f"No instrumentation runtime: {round(avg_runtime, 4)}")


    # wc_normal = open("test_time_user_static.txt", "r")
    # numbers = parse_time_info(wc_normal)
    # avg_runtime = sum(numbers) / len(numbers)
    # print(f"Static superblock formation runtime: {round(avg_runtime, 4)}")

    # wc_normal = open("test_time_user_profile.txt", "r")
    # numbers = parse_time_info(wc_normal)
    # avg_runtime = sum(numbers) / len(numbers)
    # print(f"Profile superblock formation runtime: {round(avg_runtime, 4)}")

    # wc_normal = open("test_time_user_mixed.txt", "r")
    # numbers = parse_time_info(wc_normal)
    # avg_runtime = sum(numbers) / len(numbers)
    # print(f"Mixed superblock formation runtime: {round(avg_runtime, 4)}")
