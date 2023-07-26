print("Hello, World from thread number one!\n")

for(local i = 0; i < 10; ++i) {
    print(g_counter + " --- Thread1: " + i + "\n")
    g_counter += 1
    ::suspend()
}

/* EOF */
