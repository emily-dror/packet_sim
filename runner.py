#!/usr/bin/python3

import subprocess

def compile_sim():
    subprocess.run(["make", "simulator"])

def run_sim(T):
    cmd = ["./simulator", str(T), "1", "1", "1", "9", "5", "12"]
    return subprocess.run(cmd, capture_output=True, text=True)

def clean_env():
    subprocess.run(["make", "clean"])

def main():
    compile_sim()

    RUNS = 10
    print("\nT", "E(T)")
    for T in [10, 100, 500, 1000, 1500, 2000, 2500]:
        avg_waiting = 0
        avg_dropped = 0
        for _ in range(RUNS):
            result = run_sim(T)
            avg_waiting += float(result.stdout.split()[5])
            avg_dropped += float(result.stdout.split()[2])

        avg_waiting /= RUNS
        avg_dropped /= RUNS

        print(T, "%.3f" % avg_waiting, avg_dropped)
    print()
    clean_env()

if __name__ == "__main__":
    main()