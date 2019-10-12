#include "StdAfx.h"
#include ".\meshsubdivision.h"

//--------------------------------------------------------------
#define PI 3.1415926536
#include <vector>

Mesh3D* MeshSubdivision::Doo_Sabin()
{
	if (m_pmesh == NULL)
	{
		return NULL;
	}

	Mesh3D* New_mesh = new Mesh3D;

	////////////////////////////////////////////////

	std::vector<int> vert_id_list;
	vert_id_list.assign(m_pmesh->get_num_of_edges_list(), 0);

	// ==============================================================
	// step 1 : 计算并插入新生成的点，并插入面对应的面F-Faces ,对于每个面:
	//          (1) 计算新的顶点
	//          (2) 向新网格中插入顶点
	//          (3) 插入面对应的面F-Faces
	//          (4) 建立旧网格中的半边与新网格顶点的id对应关系
	// ==============================================================
	PTR_FACE_LIST face_list = m_pmesh->get_faces_list();
	for (FACE_ITER fiter = face_list->begin(); fiter != face_list->end(); fiter++)
	{
		// 获取边的数量
		int n = (*fiter)->valence;
		float alpha;
		// 记录当前面的第一条边
		HE_edge* anchor_edge = (*fiter)->edge;
		HE_edge* temp_edge = (*fiter)->edge;
		// 遍历当前面的所有边
		VERTEX_LIST* v_list = new VERTEX_LIST;
		do
		{
			float new_x = 0, new_y = 0, new_z = 0;
			// 当前边所指向的顶点
			HE_vert* cur_edge_vert = temp_edge->vert;
			// 从当前边开始遍历顶点，计算新顶点的坐标
			HE_edge* alpha_edge = temp_edge;

			int i = 0;
			do
			{
				alpha = (i == 0 ? n + 5 : 3 + 2 * std::cos(2 * PI * i / n)) / (4 * n * 1.0);

				new_x += alpha * cur_edge_vert->x;
				new_y += alpha * cur_edge_vert->y;
				new_z += alpha * cur_edge_vert->z;

				i++;
				// 迭代计算下一个顶点相对新顶点的权重
				alpha_edge = alpha_edge->next;
				cur_edge_vert = alpha_edge->vert;
			} while (alpha_edge != temp_edge);

			// 将计算出的新顶点插入新网格
			HE_vert* new_vert = New_mesh->insert_vertex(new_x, new_y, new_z);
			// 半边和新顶点的数量是一一对应的，建立旧网格半边与新顶点的id对应关系
			vert_id_list[temp_edge->id] = new_vert->id;

			// 记录当前F-face所对应的点
			v_list->push_back(new_vert);

			temp_edge = temp_edge->next;
		} while (temp_edge != anchor_edge);
		// 插入当前面所对应的F-faces
		New_mesh->insert_face(*v_list);
		delete v_list;
	}

	// ============================================
	// step 2 : 遍历所有点，构建并插入点对应的面V-faces
	// ============================================

	PTR_VERTEX_LIST vert_list = m_pmesh->get_vertices_list();
	for (VERTEX_ITER viter = vert_list->begin(); viter != vert_list->end(); viter++)
	{
		VERTEX_LIST* v_list = new VERTEX_LIST;
		// 拿到指向该顶点的其中一条半边
		HE_edge* first_edge = (*viter)->edge->pair;
		HE_edge* temp_edge = first_edge;
		// 遍历所有指向该顶点的半边
		do
		{
			// 获取这个半边所对应的新顶点id
			int vert_id = vert_id_list[temp_edge->id];
			v_list->push_back(New_mesh->get_vertex(vert_id));
			temp_edge = temp_edge->next->pair;
		} while (temp_edge != first_edge);
		// 遍历顺序生成的是反向面，需要倒序处理一下
		for (int i = 0; i < v_list->size() / 2; i++)
		{
			HE_vert* temp = (*v_list)[i];
			(*v_list)[i] = (*v_list)[v_list->size() - 1 - i];
			(*v_list)[v_list->size() - 1 - i] = temp;
		}
		// 插入当前点所对应的V-faces
		New_mesh->insert_face(*v_list);
		delete v_list;
	}

	// ============================================
	// step 3 : 遍历所有边，构建并插入边对应的面E-faces
	//               | | <----------
	//            d  | |  a
	//            c  | |  b
	//   ----------> | |
	// ============================================

	PTR_EDGE_LIST edge_list = m_pmesh->get_edges_list();
	for (EDGE_ITER eiter = edge_list->begin(); eiter != edge_list->end(); eiter++)
	{
		HE_edge* curEdge = *eiter;
		HE_edge* nextEdge = curEdge->next;
		HE_edge* nextEdgePair = nextEdge->pair;
		HE_edge* nextEdgePairPre = nextEdgePair;
		// 寻找前一条边
		while (nextEdgePairPre->next != nextEdgePair)
			nextEdgePairPre = nextEdgePairPre->next;
		// 构造新点
		VERTEX_LIST* v_list = new VERTEX_LIST;
		v_list->push_back(New_mesh->get_vertex(vert_id_list[curEdge->id]));
		v_list->push_back(New_mesh->get_vertex(vert_id_list[nextEdge->id]));
		v_list->push_back(New_mesh->get_vertex(vert_id_list[nextEdgePairPre->id]));
		v_list->push_back(New_mesh->get_vertex(vert_id_list[nextEdgePair->id]));
		for (int i = 0; i < v_list->size() / 2; i++)
		{
			HE_vert* temp = (*v_list)[i];
			(*v_list)[i] = (*v_list)[v_list->size() - 1 - i];
			(*v_list)[v_list->size() - 1 - i] = temp;
		}
		// 插入当前边所对应的E-faces
		New_mesh->insert_face(*v_list);
		delete v_list;
	}
	/////////////////////////////////////////////////

	New_mesh->update_mesh();

	delete m_pmesh;
	m_pmesh = NULL;
	return New_mesh;
}

//--------------------------------------------------------------

Mesh3D* MeshSubdivision::Catmull_Clark()
{

	if (m_pmesh == NULL)
	{
		return NULL;
	}

	Mesh3D* New_mesh = new Mesh3D;
	// 初始化id映射数组
	std::vector<int> f_vert_id_list, e_vert_id_list, v_vert_id_list;
	f_vert_id_list.assign(m_pmesh->get_num_of_faces_list(), 0);
	e_vert_id_list.assign(m_pmesh->get_num_of_edges_list(), 0);
	v_vert_id_list.assign(m_pmesh->get_num_of_vertices_list(), 0);
	//==================================================
	// Step 1 : 对于旧网格每个面，生成新的面顶点，并插入新网格
	//==================================================
	PTR_FACE_LIST face_list = m_pmesh->get_faces_list();
	for (FACE_ITER fiter = face_list->begin(); fiter != face_list->end(); fiter++)
	{
		float new_x = 0, new_y = 0, new_z = 0;
		HE_edge* firstEdge = (*fiter)->edge;
		HE_edge* curEdge = firstEdge;
		int n = (*fiter)->valence;
		do
		{
			HE_vert* curVert = curEdge->vert;
			new_x += curVert->x * 1.0 / n;
			new_y += curVert->y * 1.0 / n;
			new_z += curVert->z * 1.0 / n;

			curEdge = curEdge->next;
		} while (firstEdge != curEdge);
		// 将计算出的面顶点插入新网格
		HE_vert* new_vert = New_mesh->insert_vertex(new_x, new_y, new_z);
		// 存储面与新面顶点之间的id对应关系
		f_vert_id_list[(*fiter)->id] = new_vert->id;
	}

	//==================================================
	// Step 2 : 对于旧网格每个边，生成新的边顶点，并插入新网格
	//==================================================
	PTR_EDGE_LIST edge_list = m_pmesh->get_edges_list();
	// 初始化标记位
	for (EDGE_ITER eiter = edge_list->begin(); eiter != edge_list->end(); eiter++)
	{
		(*eiter)->tag = true;
	}
	for (EDGE_ITER eiter = edge_list->begin(); eiter != edge_list->end(); eiter++)
	{
		if ((*eiter)->tag)
		{
			float new_x = 0, new_y = 0, new_z = 0;
			HE_edge* curEdge = *eiter;
			HE_edge* pairEdge = curEdge->pair;
			VERTEX_LIST* v_list = new VERTEX_LIST;
			// 先获取旧网格的顶点
			v_list->push_back(curEdge->vert);
			v_list->push_back(pairEdge->vert);
			// 获取新的面顶点
			if (!m_pmesh->isborder(curEdge))
			{
				v_list->push_back(New_mesh->get_vertex(f_vert_id_list[curEdge->face->id]));
				v_list->push_back(New_mesh->get_vertex(f_vert_id_list[pairEdge->face->id]));
			}	
			int dd = v_list->size();
			// 计算新顶点
			for (int i = 0; i < dd; i++)
			{
				new_x += (*v_list)[i]->x;
				new_y += (*v_list)[i]->y;
				new_z += (*v_list)[i]->z;
			}
			new_x /= dd;
			new_y /= dd;
			new_z /= dd;
			// 将计算出的边顶点插入新网格
			HE_vert* new_vert = New_mesh->insert_vertex(new_x, new_y, new_z);
			// 存储边与新边顶点之间的id对应关系
			e_vert_id_list[curEdge->id] = new_vert->id;
			e_vert_id_list[pairEdge->id] = new_vert->id;
			// 两个半边只生成一个边顶点
			curEdge->tag = false;
			pairEdge->tag = false;
		}
	}
	//===================================================
	// Step 3 : 对于旧网格每个顶点，生成新的顶点，并插入新网格
	//          (1) 边界顶点对应公式 => 3/4S + 1/8V1 + 1/8V2
	//          (2) 非边界顶点对应公式 => (Q + 2R + (n−3)S) / n
	//===================================================
	PTR_VERTEX_LIST vert_list = m_pmesh->get_vertices_list();
	for (VERTEX_ITER viter = vert_list->begin(); viter != vert_list->end(); viter++)
	{
		// S为原顶点
		HE_vert* S = *viter;
		//n是该顶点相邻的边的数目
		int n = S->degree;

		float new_x = 0, new_y = 0, new_z = 0;

		if (m_pmesh->isborder(S))
		{
			// =============== (1) 边界顶点 ==================
			HE_edge* v_edge = S->edge;
			HE_vert* V1;
			HE_vert* V2;
			// 外侧半边
			if (v_edge->pair != NULL && v_edge->face == NULL)
			{
				V1 = v_edge->vert;
				HE_edge* temp = v_edge->pair->next;
				V2 = temp->pair->next->vert;
			}
			// 内侧半边
			else if (v_edge->pair != NULL && v_edge->pair->face == NULL)
			{
				V1 = v_edge->vert;
				HE_edge* temp = v_edge->next->next->next;
				V2 = temp->pair->next->next->vert;
			}
			// 中间半边
			else
			{
				V1 = v_edge->pair->next->vert;
				V2 = v_edge->next->next->vert;
			}
			new_x = S->x * 3 / 4 + V1->x * 1 / 8 + V2->x * 1 / 8;
			new_y = S->y * 3 / 4 + V1->y * 1 / 8 + V2->y * 1 / 8;
			new_z = S->z * 3 / 4 + V1->z * 1 / 8 + V2->z * 1 / 8;
		}
		else 
		{
			// =============== (2) 非边界顶点 ==================
			
			// Q是该顶点相邻的面的新顶点的平均
			HE_vert* Q = new HE_vert(0.0, 0.0, 0.0);
			// R是该顶点的相邻边的中点的平均
			HE_vert* R = new HE_vert(0.0, 0.0, 0.0);
			// 构造Q和R
			HE_edge* firstEdge = S->edge;
			HE_edge* curEdge = S->edge;
			do
			{
				// 累加面顶点
				HE_vert* f_vert = New_mesh->get_vertex(f_vert_id_list[curEdge->face->id]);
				Q->x += f_vert->x; Q->y += f_vert->y; Q->z += f_vert->z;
				// 累加相邻边中点
				HE_vert* s_vert = curEdge->vert;
				HE_vert* e_vert = curEdge->pair->vert;
				R->x += (s_vert->x + e_vert->x) / 2;
				R->y += (s_vert->y + e_vert->y) / 2;
				R->z += (s_vert->z + e_vert->z) / 2;

				curEdge = curEdge->pair->next;
			} while (curEdge != firstEdge);
			// 计算新顶点
			Q->x /= n; Q->y /= n; Q->z /= n;
			R->x /= n; R->y /= n; R->z /= n;

			new_x = (Q->x + 2 * R->x + (n - 3) * S->x) / n;
			new_y = (Q->y + 2 * R->y + (n - 3) * S->y) / n;
			new_z = (Q->z + 2 * R->z + (n - 3) * S->z) / n;
		}
		// 插入新顶点
		HE_vert* new_vert = New_mesh->insert_vertex(new_x, new_y, new_z);
		// 存储旧顶点和新顶点之间的id对应关系
		v_vert_id_list[S->id] = new_vert->id;
	}
	//===================================================
	// Step 4 : 开始构造面
	//          (1)一个新的面顶点和与包围该面的新的边顶点连接
	//          (2)一个新的顶点顶点和与该顶点相邻的新的边顶点连接
	//===================================================
	for (FACE_ITER fiter = face_list->begin(); fiter != face_list->end(); fiter++)
	{
		HE_vert* f_vert = New_mesh->get_vertex(f_vert_id_list[(*fiter)->id]);
		HE_edge* firstEdge = (*fiter)->edge;
		HE_edge* curEdge = (*fiter)->edge;
		do
		{
			// 构造并插入新的面
			VERTEX_LIST* v_list = new VERTEX_LIST;
			v_list->push_back(New_mesh->get_vertex(e_vert_id_list[curEdge->id]));
			v_list->push_back(New_mesh->get_vertex(v_vert_id_list[curEdge->vert->id]));
			v_list->push_back(New_mesh->get_vertex(e_vert_id_list[curEdge->next->id]));
			v_list->push_back(f_vert);
			New_mesh->insert_face(*v_list);

			curEdge = curEdge->next;
		} while (firstEdge != curEdge);
	}
	/////////////////////////////////////////////////
	New_mesh->update_mesh();

	delete m_pmesh;
	m_pmesh = NULL;
	return New_mesh;
}