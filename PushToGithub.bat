@echo off
call git rm -r --cached .
call git add -A .
call git commit -m 'test1'
call git push --force