TARGET := mintest_example
CC := gcc
CFLAGS := -O1 -g -Wall -Wextra -Werror -std=c99
LDFLAGS := -lm

EXPECTED_RESULT := 8

.PHONY: build
build: $(TARGET)

.PHONY: clean
clean:
	rm -f $(TARGET)

.PHONY: test
test: build
	@./$(TARGET); \
	RESULT=$$?; \
	if [ $$RESULT -ne $(EXPECTED_RESULT) ]; then \
		echo "Unexpected exit code: $(EXPECTED_RESULT) expected but was $$RESULT"; \
		exit 1; \
	else \
		exit 0; \
	fi

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)
