#!/bin/sh

expected_exit_code=5 # 当前示例代码中共5个fail的测试用例
./mintest_example
if [ $? -ne $expected_exit_code ]; then
    echo "Unexpected exit code: $expected_exit_code expected but was $?"
    exit 1
fi
exit 0
