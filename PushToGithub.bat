@echo off
call git rm -r --cached .
call git add -A .
call git commit -m "No Comment"
call git push --force