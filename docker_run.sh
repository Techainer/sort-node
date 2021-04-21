DOCKER_NAME=sort

docker run \
    -it --rm \
    -v $PWD:/sort-node/ \
    --name sort-node \
    $DOCKER_NAME /bin/bash