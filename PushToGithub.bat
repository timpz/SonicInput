@echo off
call git rm -r --cached .
call git add -A .
call git commit -m 'nocomment'
call git push --force