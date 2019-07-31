# Why awk?
#
# I'm using awk for tests because it:
#
#   a. is ubiquitous
#   b. allows \0 in variables, unlike bash
#   c. has great string handling, unlike c
#
# I also just really like awk. A lot of people overlook awk or find
# it confusing. But it's actually a really clean language if you're
# not using the annoyingly named built-in variables. But event if
# you are, it's not any worse than bash in that respect.
#
# The one annoying thing about awk is that it doesn't have a true
# way to slurp stdout without line-buffering, which discards line-
# endings. In this case though, I know for a fact that the endings
# are \n because that's what htq outputs, so it's not a problem and
# I can wrap it up in the run function.


# Run a shell command and return its output
function run(cmd) {
    line_no = 0;
    output = "";
    while ((cmd | getline line) > 0) {
        if (line_no++ > 0) output = output "\n";
        output = output line;
    }
    return output;
}

# Test that the output of htq with given args matches the expected string
function test(name, args, expected) {
    output = run("../htq " args);
    if (output == expected) {
        print name ": " color_green "succeded" color_reset;
        total_succeded++;
    }
    else {
        print name ": " color_red "failed" color_reset;
        print "expected:\n" expected;
        print "got:\n" output;
        total_failed++;
    }
}

# Test that the status code of htq with given args matches the expected code
function test_status(name, args, expected) {
    status = system("../htq " args " 2>/dev/null >/dev/null");
    if (status == expected) {
        print name ": " color_green "succeded" color_reset;
        total_succeded++;
    }
    else {
        print name ": " color_red "failed" color_reset;
        total_failed++;
    }
}

BEGIN {

    color_green = "\033[32m";
    color_red = "\033[31m";
    color_reset = "\033[0m";

    total_failed = 0;
    total_succeded = 0;

    test("plain css selector", "p test.html", \
        "<p>Oh how I wish I could shut up like a telescope!</p>\n" \
        "<p>I think I could, if I only knew how to begin.</p>");

    test("plain css selector arg", "-c p test.html", \
        "<p>Oh how I wish I could shut up like a telescope!</p>\n" \
        "<p>I think I could, if I only knew how to begin.</p>");

    test("plain css selector print0", "-0 p test.html", \
        "<p>Oh how I wish I could shut up like a telescope!</p>\0" \
        "<p>I think I could, if I only knew how to begin.</p>\0");

    test("text extraction", "-t p test.html", \
        "Oh how I wish I could shut up like a telescope!\n" \
        "I think I could, if I only knew how to begin.");

    test("text extraction print0", "-t0 p test.html", \
        "Oh how I wish I could shut up like a telescope!\0" \
        "I think I could, if I only knew how to begin.\0");

    test("attr", "a -a href test.html", \
        "#link-1\n" \
        "#link-2");

    test("attr print0", "a -0a href test.html", \
        "#link-1\0" \
        "#link-2\0");

    test("count 1 file", "-C p test.html", "2");

    test("count 2 files", "-C p test.html test.html", \
        "test.html: 2\n" \
        "test.html: 2");

    test("count 2 files print0", "-0C p test.html test.html", \
        "test.html: 2\0" \
        "test.html: 2\0");

    test("list files", "-l p test.html test.html", \
        "test.html\n" \
        "test.html");

    test("-H suppresses -C list", "-HC p test.html test.html", "4");

    test("-h forces -C list", "-hC p test.html", "test.html: 2");


    test_status("return 1 on no matches", "aside test.html", 1);
    test_status("return 0 on matches", "p test.html", 0);
    test_status("return 1 on missing file", "p nothing-here.html", 1);

    print("");
    print(total_failed " failed");
    print(total_succeded " succeded");

}
