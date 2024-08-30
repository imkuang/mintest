#!/bin/sh

expected_exit_code=8 # 当前示例代码中fail测试用例的个数
./mintest_example
if [ $? -ne $expected_exit_code ]; then
    echo "Unexpected exit code: $expected_exit_code expected but was $?"
    exit 1
fi
exit 0
