print("Hello, World from thread number two!\n")

for(local i = 0; i < 10; ++i) {
    print(g_counter + " +++ Thread2: " + i + "\n")
    g_counter += 1
    ::suspend()
}

/* EOF */
