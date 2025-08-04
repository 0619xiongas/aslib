@echo off
set PROTOC=protoc.exe
set PROTO_DIR=.
set OUT_DIR=../netcpp/

for %%f in (.\*.proto) do (
    protoc.exe --cpp_out=../netcpp/ %%f
)
pause