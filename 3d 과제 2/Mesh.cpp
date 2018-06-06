//-----------------------------------------------------------------------------
// File: CGameObject.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Mesh.h"

CMesh::CMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, TCHAR *pstrFileName)
{
	if (pstrFileName) LoadMeshFromFile(pd3dDevice, pd3dCommandList, pstrFileName);
}

CMesh::~CMesh()
{
	if (m_pxmf3Positions) delete[] m_pxmf3Positions;
	if (m_pxmf3Normals) delete[] m_pxmf3Normals;
	if (m_pxmf2TextureCoords) delete[] m_pxmf2TextureCoords;

	if (m_pnIndices) delete[] m_pnIndices;

	if (m_pd3dVertexBufferViews) delete[] m_pd3dVertexBufferViews;

	if (m_pd3dPositionBuffer) m_pd3dPositionBuffer->Release();
	if (m_pd3dNormalBuffer) m_pd3dNormalBuffer->Release();
	if (m_pd3dTextureCoordBuffer) m_pd3dTextureCoordBuffer->Release();
	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();
}

void CMesh::ReleaseUploadBuffers() 
{
	if (m_pd3dPositionUploadBuffer) m_pd3dPositionUploadBuffer->Release();
	if (m_pd3dNormalUploadBuffer) m_pd3dNormalUploadBuffer->Release();
	if (m_pd3dTextureCoordUploadBuffer) m_pd3dTextureCoordUploadBuffer->Release();
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();

	m_pd3dPositionUploadBuffer = NULL;
	m_pd3dNormalUploadBuffer = NULL;
	m_pd3dTextureCoordUploadBuffer = NULL;
	m_pd3dIndexUploadBuffer = NULL;
};

void CMesh::Render(ID3D12GraphicsCommandList *pd3dCommandList)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dCommandList->IASetVertexBuffers(m_nSlot, m_nVertexBufferViews, m_pd3dVertexBufferViews);
	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}

void CMesh::LoadMeshFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, WCHAR *pstrFileName)
{
#ifdef _WITH_TEXT_MESH
	wifstream InFile(pstrFileName);

	TCHAR pstrToken[64] = { '\0' };

	for ( ; ; )
	{
		InFile >> pstrToken;
		if (!InFile) break;

		if (!_tcscmp(pstrToken, _T("<Vertices>:")))
		{
			InFile >> m_nVertices;
			m_pxmf3Positions = new XMFLOAT3[m_nVertices];
			for (UINT i = 0; i < m_nVertices; i++) InFile >> m_pxmf3Positions[i].x >> m_pxmf3Positions[i].y >> m_pxmf3Positions[i].z;
		}
		else if (!_tcscmp(pstrToken, _T("<Normals>:")))
		{
			InFile >> pstrToken;
			m_pxmf3Normals = new XMFLOAT3[m_nVertices];
			for (UINT i = 0; i < m_nVertices; i++) InFile >> m_pxmf3Normals[i].x >> m_pxmf3Normals[i].y >> m_pxmf3Normals[i].z;
		}
		else if (!_tcscmp(pstrToken, _T("<TextureCoords>:")))
		{
			InFile >> pstrToken;
			m_pxmf2TextureCoords = new XMFLOAT2[m_nVertices];
			for (UINT i = 0; i < m_nVertices; i++) InFile >> m_pxmf2TextureCoords[i].x >> m_pxmf2TextureCoords[i].y;
		}
		else if (!_tcscmp(pstrToken, _T("<Indices>:")))
		{
			InFile >> m_nIndices;
			m_pnIndices = new UINT[m_nIndices];
			for (UINT i = 0; i < m_nIndices; i++) InFile >> m_pnIndices[i];
		}
	}
#else
	FILE *pFile = NULL;
	::_wfopen_s(&pFile, pstrFileName, L"rb");
	::rewind(pFile);

//	char pstrToken[64] = { '\0' };
//	UINT nReadBytes = (UINT)::fread(pstrToken, sizeof(char), 12, pFile); //"<Vertices>:"
	UINT nReadBytes = (UINT)::fread(&m_nVertices, sizeof(int), 1, pFile);
	m_pxmf3Positions = new XMFLOAT3[m_nVertices];
	nReadBytes = (UINT)::fread(m_pxmf3Positions, sizeof(float), 3 * m_nVertices, pFile);

//	nReadBytes = (UINT)::fread(pstrToken, sizeof(char), 11, pFile); //"<Normals>:"
	nReadBytes = (UINT)::fread(&m_nVertices, sizeof(int), 1, pFile);
	m_pxmf3Normals = new XMFLOAT3[m_nVertices];
	nReadBytes = (UINT)::fread(m_pxmf3Normals, sizeof(float), 3 * m_nVertices, pFile);

//	nReadBytes = (UINT)::fread(pstrToken, sizeof(char), 17, pFile); //"<TextureCoords>:"
	nReadBytes = (UINT)::fread(&m_nVertices, sizeof(int), 1, pFile);
	m_pxmf2TextureCoords = new XMFLOAT2[m_nVertices];
	nReadBytes = (UINT)::fread(m_pxmf2TextureCoords, sizeof(float), 2 * m_nVertices, pFile);

//	nReadBytes = (UINT)::fread(pstrToken, sizeof(char), 11, pFile); //"<Indices>:"
	nReadBytes = (UINT)::fread(&m_nIndices, sizeof(int), 1, pFile);
	m_pnIndices = new UINT[m_nIndices];
	nReadBytes = (UINT)::fread(m_pnIndices, sizeof(UINT), m_nIndices, pFile);

	::fclose(pFile);
#endif

	m_pd3dPositionBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Positions, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);
	m_pd3dNormalBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf3Normals, sizeof(XMFLOAT3) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dNormalUploadBuffer);
	m_pd3dTextureCoordBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pxmf2TextureCoords, sizeof(XMFLOAT2) * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dTextureCoordUploadBuffer);

	m_nVertexBufferViews = 3;
	m_pd3dVertexBufferViews = new D3D12_VERTEX_BUFFER_VIEW[m_nVertexBufferViews];

	m_pd3dVertexBufferViews[0].BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[0].StrideInBytes = sizeof(XMFLOAT3);
	m_pd3dVertexBufferViews[0].SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	m_pd3dVertexBufferViews[1].BufferLocation = m_pd3dNormalBuffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[1].StrideInBytes = sizeof(XMFLOAT3);
	m_pd3dVertexBufferViews[1].SizeInBytes = sizeof(XMFLOAT3) * m_nVertices;

	m_pd3dVertexBufferViews[2].BufferLocation = m_pd3dTextureCoordBuffer->GetGPUVirtualAddress();
	m_pd3dVertexBufferViews[2].StrideInBytes = sizeof(XMFLOAT2);
	m_pd3dVertexBufferViews[2].SizeInBytes = sizeof(XMFLOAT2) * m_nVertices;

	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, m_pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
}

