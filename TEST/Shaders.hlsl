//���� ���̴��� �����Ѵ�. 

//float4 VSMain(uint nVertexID : SV_VertexID) : SV_POSITION
//{
//	float4 output;
////������Ƽ��(�ﰢ��)�� �����ϴ� ������ �ε���(SV_VertexID)�� ���� ������ ��ȯ�Ѵ�. //������ ��ġ ��ǥ�� ��ȯ�� �� ��ǥ(SV_POSITION)�̴�. ��, ������ǥ���� ��ǥ�̴�. 
//if (nVertexID == 0) output = float4(0.0, 0.5, 0.5, 1.0);
//else if (nVertexID == 1) output = float4(0.5, -0.5, 0.5, 1.0);
//else if (nVertexID == 2) output = float4(-0.5, -0.5, 0.5, 1.0);
//return(output);
//}
//
////�ȼ� ���̴��� �����Ѵ�. 
//float4 PSMain(float4 input : SV_POSITION) : SV_TARGET
//{
//	//������Ƽ���� ��� �ȼ��� ������ ��������� ��ȯ�Ѵ�. 
//	return(float4(1.0f, 1.0f, 0.0f, 1.0f)); //Yellow
//}


//���� ���̴��� �Է��� ���� ����ü�� �����Ѵ�.
struct VS_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};


//���� ���̴��� ���(�ȼ� ���̴��� �Է�)�� ���� ����ü�� �����Ѵ�. 
struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

//���� ��ü�� ������ ���� ��� ���۸� �����Ѵ�. 
cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxWorld : packoffset(c0);
};

//��Ʈ �ñ׳�ó �Ķ��Ÿ �Ű�����
//ī�޶��� ������ ���� ��� ���۸� �����Ѵ�. 
cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


////���� ���̴��� �����Ѵ�. 
//VS_OUTPUT VSMain(VS_INPUT input)
//{
//	VS_OUTPUT output;
//	//������ ��ġ ���ʹ� ������ǥ��� ǥ���Ǿ� �����Ƿ� ��ȯ���� �ʰ� �״�� ����Ѵ�.
//	output.position = float4(input.position, 1.0f);
//	//�ԷµǴ� �ȼ��� ����(�����Ͷ����� �ܰ迡�� �����Ͽ� ���� ����)�� �״�� ����Ѵ�. 
//	output.color = input.color;
//	return(output);
//}
////
//
////�ȼ� ���̴��� �����Ѵ�. 
//float4 PSMain(VS_OUTPUT input) : SV_TARGET
//{
//	//�ԷµǴ� �ȼ��� ������ �״�� ���-���� �ܰ�(���� Ÿ��)�� ����Ѵ�. 
//	return(input.color);
//}


//���� ���̴��� �����Ѵ�. 
VS_OUTPUT VSDiffused(VS_INPUT input)
{
	VS_OUTPUT output;
	//������ ��ȯ(���� ��ȯ, ī�޶� ��ȯ, ���� ��ȯ)�Ѵ�. 
	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView),
		gmtxProjection);
	output.color = input.color;
	return(output);
}


//�ȼ� ���̴��� �����Ѵ�. 
float4 PSDiffused(VS_OUTPUT input) : SV_TARGET
{
	return(input.color);
}

//
//float4 main(float4 pos : POSITION) : SV_POSITION
//{
//	return pos;
//}