import sys

def memory_test():
    #size test in bytes
    number = 52
    float_number = 52.0
    hello_string = "Hello World"
    print(sys.getsizeof(number))
    print(sys.getsizeof(float_number))
    print(sys.getsizeof(hello_string))

if __name__ == '__main__':
    memory_test()