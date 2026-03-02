FROM python:3.11

WORKDIR /app

COPY . .

RUN pip install grpcio grpcio-tools

# generate python grpc code inside python_client folder
RUN python -m grpc_tools.protoc \
    -I protos \
    --python_out=python_client \
    --grpc_python_out=python_client \
    protos/zenith.proto

WORKDIR /app/python_client

CMD ["python", "zenith_client.py"]