set -e
cd "$(dirname "$0")"

cat > program.in
./mx++ ./program.in --semantic