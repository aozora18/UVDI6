
/*
 desc : ���� ���õ� Job�� XML ������ ����
*/

#include "pch.h"
#include "MainApp.h"
#include "JobSelectXml.h"
#include "../UVDI15/GlobalVariables.h"

#include <iostream>
#include "pugixml.hpp"
#include <vector>
#include <algorithm>
#include <fstream>  
#include <iostream>
#include <iosfwd>
#include <string>
#include <sstream>
#include <tuple>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <filesystem>
namespace fs = std::filesystem;

struct LocalData
{
	int index;
	double orgX, orgY;
	int bmpX, bmpY;
	double gbrX, gbrY;
};

enum SearchDir
{
	XIncrease = 0b00001010,
	XDecrease = 0b00001000,
	YIncrease = 0b00000110,
	YDecrease = 0b00000100,
};


int ProcessXml(string xmlFilename)
{

	auto SplitPath = [&](const std::string& filepath, std::string& directory, std::string& filename)
	{
		fs::path pathObj(filepath);
		directory = pathObj.parent_path().string();
		filename = pathObj.filename().string();
	};

	auto AddSuffixToFilename = [&](const std::string& filename, const std::string& suffix)->std::string
	{
		size_t dotPos = filename.find_last_of('.');
		if (dotPos != std::string::npos)
			return filename.substr(0, dotPos) + suffix + filename.substr(dotPos);
		return filename;
	};

	auto Backup = [&](const std::string& srcfilename, string& copyFileName)
	{
		std::string backupFilename = (copyFileName.length() == 0 ? srcfilename + ".bak" : copyFileName);
		fs::copy_file(srcfilename, backupFilename, fs::copy_options::overwrite_existing);
	};


	auto CheckProcessed = [&](string xmlFilename) -> bool
	{
		std::ifstream file(xmlFilename);
		std::string xmlData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

		// pugi::xml_document ��ü ���� �� XML ������ �ε�
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_string(xmlData.c_str());

		if (!result) return false;

		string xmlString = xmlData.c_str();

		pugi::xml_node rltNode = doc.child("rlt");
		if (!rltNode) return 0;

		pugi::xml_node fidNode = rltNode.child("Fiducials");
		if (!rltNode)  return 0;

		bool processed = fidNode.attribute("processed").as_bool();

		return processed;
	};

	auto GetLocalfiducialCnt = [&](string xmlFilename) -> int
	{
		std::ifstream file(xmlFilename);
		std::string xmlData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

		// pugi::xml_document ��ü ���� �� XML ������ �ε�
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_string(xmlData.c_str());

		if (!result) return 0;

		string xmlString = xmlData.c_str();

		pugi::xml_node rltNode = doc.child("rlt");
		if (!rltNode) return 0;

		pugi::xml_node fidNode = rltNode.child("Fiducials");
		if (!rltNode)  return 0;

		pugi::xml_node localNode = fidNode.child("Local");
		if (!localNode)  return 0;

		int localCount = localNode.attribute("number").as_int();
		return localCount;
	};


	auto GetXMLString = [&](string xmlFilename, string& xmlString)
	{
		std::ifstream file(xmlFilename);
		std::string xmlData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

		// pugi::xml_document ��ü ���� �� XML ������ �ε�
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_string(xmlData.c_str());

		if (!result) return;

		xmlString = xmlData.c_str();
	};

	auto GetLocalzoneData = [&](string xmlFilename, std::vector<LocalData>& localZone )
	{
		std::ifstream file(xmlFilename);
		std::string xmlData((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

		// pugi::xml_document ��ü ���� �� XML ������ �ε�
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_string(xmlData.c_str());

		if (!result) return 0;

		pugi::xml_node rltNode = doc.child("rlt");
		if (!rltNode) return 0;

		pugi::xml_node fidNode = rltNode.child("Fiducials");
		if (!rltNode)  return 0;

		pugi::xml_node localNode = fidNode.child("Local");
		if (!localNode)  return 0;

		for (pugi::xml_node fidNode : localNode.children("fid"))
		{
			LocalData data;
			data.orgX = fidNode.child("org").attribute("x").as_double();
			data.orgY = fidNode.child("org").attribute("y").as_double();
			data.bmpX = fidNode.child("bmp").attribute("x").as_int();
			data.bmpY = fidNode.child("bmp").attribute("y").as_int();
			data.gbrX = fidNode.child("gbr").attribute("x").as_double();
			data.gbrY = fidNode.child("gbr").attribute("y").as_double();
			localZone.push_back(data);
		}
		return (int)localZone.size();
	};



	auto FindClosestItem = [&](const std::vector<LocalData>& localDataVec, int baseItemIndex, SearchDir dir)->int
		{
			int closestIndex = 0;
			double closestDistance = 0;
			const unsigned char xMask = 0b00001000;
			const unsigned char IncreaseMask = 0b00000010;
			double threahold = 0.010f;
			double values[2] = { 0, };

			double apositeCoord = (dir & xMask) == xMask ? localDataVec[baseItemIndex].gbrY : localDataVec[baseItemIndex].gbrX;
			
			double orgValue = (dir & xMask) == xMask ? localDataVec[baseItemIndex].gbrX : localDataVec[baseItemIndex].gbrY;

			for (size_t i = 0; i < localDataVec.size(); ++i)
			{
				if (i == baseItemIndex) continue;

				double distance = 0;

				double apositeCmpCoord = (dir & xMask) == xMask ? localDataVec[i].gbrY : localDataVec[i].gbrX;

				bool same = fabsf(fabsf(apositeCoord) - fabsf(apositeCmpCoord)) < threahold;

				if (!same)
					continue;

				values[1] = (dir & xMask) == xMask ? localDataVec[i].gbrX : localDataVec[i].gbrY;

				if ((dir & IncreaseMask) == IncreaseMask ? orgValue < values[1] : orgValue > values[1])
				{
					if (closestDistance == 0)
					{
						closestDistance = values[1];
						closestIndex = i;
					}
					else
					{
						if ((dir & IncreaseMask) == IncreaseMask)
						{
							if (closestDistance > values[1])
							{
								closestDistance = closestDistance > values[1];
								closestIndex = i;
							}
						}
						else
						{
							if (closestDistance < values[1])
							{
								closestDistance = values[1];
								closestIndex = i;
							}
						}
					}
				}
			}

			return closestIndex;
		};

	auto makeLocalZone = [&](vector<LocalData>& unSortpointData, vector<int>& pointIndex, vector<LocalData>& zoneStack)
	{
		std::sort(unSortpointData.begin(), unSortpointData.end(), [](const auto& a, const auto& b)
			{
				return a.gbrX < b.gbrX;
			});
		std::sort(unSortpointData.begin(), unSortpointData.end(), [](const auto& a, const auto& b)
			{
				return a.gbrY < b.gbrY;
			});

		SearchDir searchOrder[] = { SearchDir::XIncrease ,SearchDir::YIncrease ,SearchDir::XDecrease };//, SearchDir::YDecrease

		pointIndex.push_back(0);
		zoneStack.push_back(unSortpointData[pointIndex.back()]);
		for (int i = 0; i < 3; i++)
		{
			pointIndex.push_back(FindClosestItem(unSortpointData, pointIndex.back(), searchOrder[i]));
			zoneStack.push_back(unSortpointData[pointIndex.back()]);
		}
	};

	auto SetProcessedFlag = [&](std::string& xmlString)
	{
		size_t sp = xmlString.find("<Fiducials");
		if (sp != string::npos)
		{
			size_t ep = xmlString.find(">", sp);

			if (ep != string::npos)
			{
				xmlString.erase(sp, (ep - sp) + 1);
				xmlString.insert(sp, "<Fiducials processed=\"true\">");
			}
		}
	};

	auto GenerateFidXmlString = [&](const LocalData& data)->std::string
	{
		char buffer[256];

		sprintf_s(buffer,
			"\n<fid id=\"%d\">\n"
			"    <org x=\"%f\" y=\"%f\" />\n"
			"    <bmp x=\"%d\" y=\"%d\" />\n"
			"    <gbr x=\"%f\" y=\"%f\" />\n"
			"</fid>\n",
			data.index, data.orgX, data.orgY, data.bmpX, data.bmpY, data.gbrX, data.gbrY);

		return std::string(buffer);

	};

	auto CheckFilesInDirectory = [&](const std::string& directory, string token, bool& find,string& combinePath)
	{
		auto CodeBody = [&]()
		{
				try
				{
					string root, file;
					SplitPath(directory, root, file);
					auto searchName = AddSuffixToFilename(file, token);
					combinePath = root + "\\" + searchName;
					for (const auto& entry : fs::directory_iterator(root))
					{
						if (!entry.is_regular_file()) continue;
						std::string filename = entry.path().filename().string();
						if (filename != searchName) continue;
						return true;
					}
					return false;
				}
				catch (...)
				{
					return false;
				}
			
		};

		find =  CodeBody();
	};


	auto WriteString = [&](string filename, string text)
		{
			std::ofstream outputFile(filename.c_str());

			// ������ ���ȴ��� Ȯ��
			if (outputFile.is_open())
			{
				outputFile << text;
				outputFile.close();
			}
		};

	auto GenerateFid = [&](std::string xmlString, std::vector<LocalData>& zoneStack,string noLocalFilename,string withLocalFilename)
	{
		auto fidCount = zoneStack.size();

		size_t localStartPos = xmlString.find("<Local");
		size_t localEndPos = xmlString.find("</Local>");

		auto xmlstringClone = xmlString;
		xmlstringClone.erase(localStartPos, localEndPos - localStartPos);
		xmlstringClone.insert(localStartPos, "<Local d-code=\"0\" number=\"0\">");
		
		if(noLocalFilename.empty() == false)
			WriteString(noLocalFilename, xmlstringClone);
		
		if (localStartPos != std::string::npos && localEndPos != std::string::npos)
		{
			size_t fidStartPos = xmlString.find("<fid", localStartPos);
			while (fidStartPos != std::string::npos && fidStartPos < localEndPos)
			{
				size_t fidEndPos = xmlString.find("</fid>", fidStartPos);
				if (fidEndPos != std::string::npos && fidEndPos < localEndPos)
				{
					xmlString.erase(fidStartPos, fidEndPos - fidStartPos + string("</fid>").length());
				}
				fidStartPos = xmlString.find("<fid", fidStartPos);
			}
		}


		//������ʹ� �����߰��Ȱ�.

		auto addPoint = xmlString.find(">", localStartPos);

		if (addPoint != string::npos)
			addPoint += string(">").length();

		for (int i = 0; i < zoneStack.size(); i++)
		{
			zoneStack[i].index = i + 1;
			auto str = GenerateFidXmlString(zoneStack[i]);
			xmlString.insert(addPoint, str);
			cout << xmlString.c_str();
			addPoint = xmlString.find("</fid>", addPoint);
			if (addPoint != string::npos)
				addPoint += string("</fid>").length();
		}
		
		if (withLocalFilename.empty() == false)
			WriteString(withLocalFilename, xmlString);
	};


	auto CodeBody = [&]()->int
	{

		vector<LocalData> zoneStack;
		std::vector<LocalData> localZone;
		string xmlString;

		bool localFind = false; string onlyGlobalPath="";
		bool globalFind = false; string withGlobalLocal = "";

		CheckFilesInDirectory(xmlFilename, "_G", globalFind, onlyGlobalPath);
		CheckFilesInDirectory(xmlFilename, "_GL", localFind, withGlobalLocal);

		bool processed = CheckProcessed(xmlFilename);
		int localCount = GetLocalfiducialCnt(xmlFilename);
		GetXMLString(xmlFilename, xmlString);

		if (processed == false)
		{
			Backup(xmlFilename, string()); //������ϸ����

			SetProcessedFlag(xmlString);
			WriteString(xmlFilename, xmlString);

			if (localCount == 0)
			{	
				Backup(xmlFilename, onlyGlobalPath);
				Backup(xmlFilename, withGlobalLocal);
				return 1; //�� ó���Ұ� ����.
			}
			else
			{
				GetLocalzoneData(xmlFilename, localZone);
					
				while (localZone.empty() == false)
				{
					vector<int> pointIndex;

					makeLocalZone(localZone, pointIndex, zoneStack);

					std::sort(pointIndex.begin(), pointIndex.end(), std::greater<size_t>());

					for (auto indexToDelete : pointIndex)
						localZone.erase(localZone.begin() + indexToDelete);
				}

				 GenerateFid(xmlString, zoneStack, onlyGlobalPath, withGlobalLocal);

				return 1;
			}
		}
		else
		{
			return 1;
		}
		

		
	};
	
	return CodeBody();
	
}


/*
 desc : ������
 parm : None
 retn : None
*/
CJobSelectXml::CJobSelectXml()
{
	m_pFidGlobal = NULL;
	m_pFidLocal = NULL;
	m_pstPanelData = NULL;
	m_pPhDistX = NULL;
	m_u8StripeCount = 0x00;
	m_enAlignType = ENG_ATGL::en_global_0_local_0x0_n_point;

	memset(m_szJobName, 0x00, MAX_GERBER_NAME);
}

/*
 desc : �ı���
 parm : None
 retn : None
*/
CJobSelectXml::~CJobSelectXml()
{
	ResetData();
}

/*
 desc : Reset Data
 parm : None
 retn : None
*/
VOID CJobSelectXml::ResetData()
{
	if (m_pFidGlobal)	delete m_pFidGlobal;
	if (m_pFidLocal)	delete m_pFidLocal;
	if (m_pstPanelData)	delete m_pstPanelData;
	if (m_pPhDistX)		delete m_pPhDistX;

	m_pstPanelData = NULL;
	m_pFidGlobal = NULL;
	m_pFidLocal = NULL;
	m_pPhDistX = NULL;
}

/*
 desc : ���� �ݹ� �Լ� (���� ����)
		X ��ǥ �������� �����ϵ�, X ��ǥ�� �����ϸ�, Y ��ǥ ���� �������� ����
 parm : first	- [in]  ù��° �񱳸� ���� ����ü ����
		second	- [in]  �ι�° �񱳸� ���� ����ü ����
 retn : TRUE or FALSE
*/
BOOL CbfSortX(STG_XMXY& first, STG_XMXY& second)
{
	/* 2���� X ��ǥ�� �����ϸ�, Y ��ǥ ������ (Y ��ǥ�� ū ���� ���� ��ġ) ������ ���� */
	if (first.mark_x == second.mark_x)	return first.mark_y > second.mark_y;
	/* X ��ǥ ������ (X ��ǥ�� �߿��� ���� ���� ���� ��ġ) ������ ���� */
	return first.mark_x < second.mark_x;
}

/*
 desc : ���� �ݹ� �Լ� (���� ����)
		Y ��ǥ �������� �����ϵ�, Y ��ǥ�� �����ϸ�, X ��ǥ ���� �������� ����
 parm : first	- [in]  ù��° �񱳸� ���� ����ü ����
		second	- [in]  �ι�° �񱳸� ���� ����ü ����
 retn : TRUE or FALSE
*/
BOOL CbfSortY(STG_XMXY& first, STG_XMXY& second)
{
	/* 2���� Y ��ǥ�� �����ϸ�, X ��ǥ ������ (X ��ǥ�� ū ���� ���� ��ġ) ������ ���� */
	/* �����, X ��ǥ ���� �������� �� ���, Y ��ǥ�� Scan ���� ������ ���� ������ �� ���� ���� */
	if (first.mark_y == second.mark_y)	return first.mark_x > second.mark_x;
	/* Y ��ǥ ������ (Y ��ǥ�� �߿��� ū ���� ���� ��ġ) ������ ���� */
	return first.mark_y > second.mark_y;
}

/*
 desc : �Էµ� ��ǥ�� Mark 1, 2, 3, 4 �� �� ��� ��ġ�ϴ��� �Ǵ�
 parm : cent	- [in]  �߽� ��ǥ
		mark	- [in]  Mark ��ġ
 retn : Mark ��ġ
		0x1000 - 1�� ��ũ ��ġ
		0x0100 - 2�� ��ũ ��ġ
		0x0010 - 3�� ��ũ ��ġ
		0x0001 - 4�� ��ũ ��ġ
*/
UINT16 CJobSelectXml::GetMarkPosDirect(STG_XMXY mark)
{
	UINT16 u16Pos = 0x0000;

	if (m_stMarkCent.mark_x < mark.mark_x)
	{
		if (m_stMarkCent.mark_y < mark.mark_y)		u16Pos = 0x0010;	/* Right_Top */
		else if (m_stMarkCent.mark_y > mark.mark_y)	u16Pos = 0x0001;	/* Right_Bottom */
	}
	else
	{
		if (m_stMarkCent.mark_y < mark.mark_y)		u16Pos = 0x1000;	/* Left_Top */
		else if (m_stMarkCent.mark_y > mark.mark_y)	u16Pos = 0x0100;	/* Left_Bottom */
	}
	return u16Pos;
}

/*
 desc : ��ǥ ���� �۾� ����
		3�� 2���� �� Strip (���� ��, ����; stripe : �ٹ���. ��, ����) �� Local Fiducial ������ 4��
 parm : col		- [in]  Strip�� ���� ���� (���� 2��)
		row		- [in]  Strip�� ���� ���� (���� 2 ~ 6��)
		s_cnt	- [in]  Stripe ���� 1 Block ���� Fiducial ���� (���� 4���̰�, 2���� ��� Shared �����)
		shared	- [in]  Align Mark Type�� Shared Type���� ����
		x, y	- [out] �� X / Y ��ǥ�� ����� ����Ʈ ����. ��ȯ�� ��, ���ĵ� ä�� �����
 retn : TRUE or FALSE
*/
BOOL CJobSelectXml::SortMarks(UINT16 col, UINT16 row, UINT16 s_cnt, BOOL shared,
	CAtlList <STG_XMXY>& mark_xy)
{
	INT32 i, i32Next = 0, i32Loop = 0;
	INT32 i32Count = 0;
	DOUBLE dbMinX = +999999.0f;
	DOUBLE dbMaxX = -999999.0f;
	DOUBLE dbMinY = +999999.0f;
	DOUBLE dbMaxY = -999999.0f;
	LPG_XMXY pstXY = NULL, pstNext;
	STG_XMXY stXY = { STG_XMXY(), };

	map<double, double> mx, my;

	for (int i = 0; i < mark_xy.GetCount(); i++)
	{
		auto fid = mark_xy.GetAt(mark_xy.FindIndex(i));
		mx[std::floor(fid.mark_x * 100.0f) / 100.0f] = fid.mark_x;
		my[std::floor(fid.mark_y * 100.0f) / 100.0f] = fid.mark_y;
	}
	row = my.size();
	col = mx.size();
	//auto x = mx.size();
	//auto y = my.size();

	if (row * col != mark_xy.GetCount())
		return false;

	if (shared)
	{
		i32Count = row * col;	/* Shared ����� Stripe�� Block ���� 2���� Fiducial�� ������, ��ü Mark ���� ����� ���� 0 ���� ��� */
		i32Next = row;			/* Shared ����� Row ������ Block ������ ���� �ʰ�, Line �������� ���� �ǹǷ�, �� Line ���� Fiducial�� ������ */
		i32Loop = col;
	}
	else
	{
		/* Global Mark������, ��ũ ������ 3���� ��� */
		if (mark_xy.GetCount() == 3)	i32Count = 3;
		else
		{
			i32Count = row * col;// *s_cnt;
			i32Next = row;		/* 1 Strip ���� ��/�� 2���� Fiducial�� �ְ�, row ������ŭ �����Ƿ� */
			i32Loop = col;
		}
	}
	/* Align Mark Type �� �ʱ�ȭ */
	m_u16MarkType = 0x0000;
	/* X, Y ��ǥ ������ ������ ����ü �޸� �Ҵ� */
	pstXY = new STG_XMXY[i32Count];
	/* ��ü ��ϵ� X/Y ��ǥ�� ����ü �迭�� ���� */
	for (i = 0; i < i32Count; i++)
	{
		pstXY[i] = mark_xy.GetAt(mark_xy.FindIndex(i));

		if (pstXY[i].mark_x < dbMinX)	dbMinX = pstXY[i].mark_x;
		if (pstXY[i].mark_x > dbMaxX)	dbMaxX = pstXY[i].mark_x;
		if (pstXY[i].mark_y < dbMinY)	dbMinY = pstXY[i].mark_y;
		if (pstXY[i].mark_y > dbMaxY)	dbMaxY = pstXY[i].mark_y;
	}
	/* ������ �簢�� �߽� ��ǥ ���ϱ� */
	m_stMarkCent.mark_x = dbMinX + (dbMaxX - dbMinX) / 2.0f;
	m_stMarkCent.mark_y = dbMinY + (dbMaxY - dbMinY) / 2.0f;

	if (i32Count == 2)
	{
		if (pstXY[0].mark_x >= pstXY[1].mark_x)
		{
			memcpy(&stXY, &pstXY[0], sizeof(STG_DBXY));
			memcpy(&pstXY[0], &pstXY[1], sizeof(STG_DBXY));
			memcpy(&pstXY[1], &stXY, sizeof(STG_DBXY));
		}
		m_u16MarkType = 0x1010;	/* 1���� 2�� ��ũ�� ��/�� ��ġ�Ǿ� �����Ƿ� */
	}
	else if (i32Count == 3)
	{
		pstNext = pstXY;
		/* X ��ǥ <����> �������� ��ü Mark ������ŭ ���� ���� */
		std::sort(pstNext, pstNext + i32Count, CbfSortX);
		/* --------------------------- */
		/* Align Mark ��ġ Ÿ�� ���ϱ� */
		/* --------------------------- */
		for (i = 0; i < 3; i++)
		{
			m_u16MarkType |= GetMarkPosDirect(pstXY[i]);
		}
		if (0x1100 == (0x1100 & m_u16MarkType))	/* Mark 2���� ���� ������ �ִ� ��� */
		{
			/* Mark 1���� 2���� Y ��ǥ �� �� �� ���� ���� ���� */
			if (pstXY[0].mark_y < pstXY[1].mark_y)
			{
				memcpy(&stXY, &pstXY[0], sizeof(STG_XMXY));
				memcpy(&pstXY[0], &pstXY[1], sizeof(STG_XMXY));
				memcpy(&pstXY[1], &stXY, sizeof(STG_XMXY));
			}
		}
		else								/* Mark 2���� ������ ������ �ִ� ��� */
		{
			/* Mark 2���� 3���� Y ��ǥ �� �� �� ���� ���� ���� */
			if (pstXY[1].mark_y < pstXY[2].mark_y)
			{
				memcpy(&stXY, &pstXY[1], sizeof(STG_XMXY));
				memcpy(&pstXY[1], &pstXY[2], sizeof(STG_XMXY));
				memcpy(&pstXY[2], &stXY, sizeof(STG_XMXY));
			}
		}

		/* ��� ����Ʈ���� �� x���� Y ��ǥ <����> �������� ���� ���� */
		for (i = 0; i < i32Loop; i++)
		{
			std::sort(pstNext, pstNext + i32Next, CbfSortY);
			pstNext += i32Next;
		}
	}
	else
	{
		m_u16MarkType = 0x1111;	/* Global or Local Mark�� 4����  Mark�� �����ϹǷ� */
		pstNext = pstXY;
		/* X ��ǥ <����> �������� ��ü Mark ������ŭ ���� ���� */
		std::sort(pstNext, pstNext + i32Count, CbfSortX);
		/* ��� ����Ʈ���� �� x���� Y ��ǥ <����> �������� ���� ���� */
		for (i = 0; i < i32Loop; i++)
		{
			std::sort(pstNext, pstNext + i32Next, CbfSortY);
			pstNext += i32Next;
		}
	}

	/* �ٽ� ���� ����Ʈ�� ����Ͽ� ��ȯ ���� */
	mark_xy.RemoveAll();
	for (i = 0; i < i32Count; i++)
	{
		mark_xy.AddTail(pstXY[i]);
	}

	/* �ӽ÷� ������ X/Y ���� �޸� ���� */
	delete[] pstXY;

	return TRUE;
}

/*
 desc : Align Type ��ȿ�� ����
 parm : align_type	- [in]  Gerber Data�� ����� Mark Type (ENG_ATGL)
 retn : TRUE or FALSE
*/
//BOOL CJobSelectXml::IsValidAlignType(ENG_ATGL align_type)
//{
//	switch (align_type)
//	{
//	case ENG_ATGL::en_global_4_local_0x0_n_point:
//	case ENG_ATGL::en_global_3_local_0x0_n_point:
//	case ENG_ATGL::en_global_2_local_0x0_n_point:
//
//	case ENG_ATGL::en_global_4_local_2x1_n_point:
//
//	case ENG_ATGL::en_global_4_local_2x2_n_point:
//	case ENG_ATGL::en_global_4_local_3x2_n_point:
//	case ENG_ATGL::en_global_4_local_4x2_n_point:
//	case ENG_ATGL::en_global_4_local_5x2_n_point:
//
//	case ENG_ATGL::en_global_4_local_2x2_s_point:
//	case ENG_ATGL::en_global_4_local_3x2_s_point:
//	case ENG_ATGL::en_global_4_local_4x2_s_point:
//	case ENG_ATGL::en_global_4_local_5x2_s_point:
//	case ENG_ATGL::en_not_defined:	// by sysandj : Preview�� ���� �߰�
//		return TRUE;
//	}
//	return FALSE;
//}

/*
 desc : Align Type�� Shared Type���� �ƴ��� ����
 parm : align_type	- [in]  Gerber Data�� ����� Mark Type (ENG_ATGL)
 retn : TRUE or FALSE
*/
//BOOL CJobSelectXml::IsSharedAlignType(ENG_ATGL align_type)
//{
//	switch (align_type)
//	{
//	case ENG_ATGL::en_global_4_local_0x0_n_point:
//	case ENG_ATGL::en_global_3_local_0x0_n_point:
//	case ENG_ATGL::en_global_2_local_0x0_n_point:
//
//	case ENG_ATGL::en_global_4_local_2x1_n_point:
//
//	case ENG_ATGL::en_global_4_local_2x2_n_point:
//	case ENG_ATGL::en_global_4_local_3x2_n_point:
//	case ENG_ATGL::en_global_4_local_4x2_n_point:
//	case ENG_ATGL::en_global_4_local_5x2_n_point:
//	case ENG_ATGL::en_not_defined:	// by sysandj : Preview�� ���� �߰�
//		return FALSE;
//	}
//	return TRUE;
//}

/*
 desc : xml file ���� - Global & Local Fiducial
 parm : job_name- [in]  ���� ��ΰ� ���Ե� ���õ� job name�� ���
		type	- [in]  Mark ���� ���� (ENG_ATGL) (Global or Local Mark �ǹ̰� �ƴ�)
 retn : TRUE or FALS
*/
BOOL CJobSelectXml::LoadRegistrationXML(CHAR* job_name, ENG_ATGL align_type)
{
	BOOL res = TRUE;
	CHAR szJobPath[MAX_PATH_LEN] = { NULL };
	UINT16 i, u16Row = 0, u16Col = 0, u16Cnt = 0/* Stripe ���� 1 Block ���� Fiducial ���� */;
	BOOL bSharedType = FALSE;
	/* Gerber X, Y ��ǥ ���� �ӽ� ����Ʈ */
	STG_XMXY stOrgXY = { STG_XMXY(),}, stBmpXY = { STG_XMXY(), }, stGbrXY = { STG_XMXY() ,};
	CAtlList <STG_XMXY> lstGbrXY;

	tinyxml2::XMLDocument xmlDoc;
	tinyxml2::XMLError xmlErr;
	tinyxml2::XMLNode* xmlNode;

	tinyxml2::XMLElement* xmlElem1, * xmlElem2, * xmlElem3, * xmlData;
	tinyxml2::XMLAttribute* xmlAttr;

	/* Align Type ��ȿ�� �˻� */
	/*if (align_type != ENG_ATGL::en_global_0_local_0x0_n_point && !IsValidAlignType(align_type))	return FALSE;*/
	/* Create the panel object */
	ResetData();

	/* Local to Global & Local Fiducial */
	m_pFidGlobal = new CFiducialData;
	m_pFidLocal = new CFiducialData;
	ASSERT(m_pFidGlobal && m_pFidLocal);
	m_pstPanelData = new STG_XDPD[MAX_PANEL_STRING_COUNT];
	ASSERT(m_pstPanelData);
	memset(m_pstPanelData, 0x00, sizeof(STG_XDPD) * (MAX_PANEL_STRING_COUNT));
	/* Photohead ���� X ������ �Ÿ� �� �ӽ� ���� */
	m_pPhDistX = new DOUBLE[MAX_PH];
	ASSERT(m_pPhDistX);
	memset(m_pPhDistX, 0x00, sizeof(DOUBLE) * MAX_PH);
	/* job name ���� ��� ���� */
	sprintf_s(szJobPath, MAX_PATH_LEN, "%s\\rlt_settings.xml", job_name);

	// XML ����
	xmlErr = xmlDoc.LoadFile(szJobPath);
	if (0 != xmlErr)	return FALSE;

	ProcessXml(string(szJobPath));

	/* ù ��° ��� ��� */
	xmlNode = (tinyxml2::XMLNode*)xmlDoc.FirstChild();
	if (!xmlNode)	return FALSE;
	/* search for <rlt> */
	xmlNode = (tinyxml2::XMLNode*)xmlNode->NextSibling();
	if (0 != strcmp("rlt", xmlNode->Value()))
	{
		ResetData();
		return FALSE;
	}
	/* search for <BmpImage> */
	xmlElem1 = (tinyxml2::XMLElement*)xmlNode->FirstChildElement("BmpImage");
	if (!xmlElem1 || 0 != strcmp(xmlElem1->Value(), "BmpImage"))
	{
		ResetData();
		return FALSE;
	}
	/* row_size */
	xmlAttr = (tinyxml2::XMLAttribute*)xmlElem1->FirstAttribute();
#if 0	/* 1.5.0 �� ������, 2.0.0 �������ʹ� ������ */
	/* row_count */
	xmlAttr = (tinyxml2::XMLAttribute*)xmlAttr->Next();
#endif
	/* shiftX */
	xmlAttr = (tinyxml2::XMLAttribute*)xmlAttr->Next();
	/* shiftY */
	xmlAttr = (tinyxml2::XMLAttribute*)xmlAttr->Next();
	/* gerber_width */
	xmlAttr = (tinyxml2::XMLAttribute*)xmlAttr->Next();
	m_dbGerberWidth = xmlAttr->DoubleValue();
	/* gerber_height */
	xmlAttr = (tinyxml2::XMLAttribute*)xmlAttr->Next();
	m_dbGerberHeight = xmlAttr->DoubleValue();
	/* scale x */
	xmlAttr = (tinyxml2::XMLAttribute*)xmlAttr->Next();
	m_dbScaleX = xmlAttr->DoubleValue();
	/* scale y */
	xmlAttr = (tinyxml2::XMLAttribute*)xmlAttr->Next();
	m_dbScaleY = xmlAttr->DoubleValue();

	/* search for <Stripes> */
	xmlElem1 = (tinyxml2::XMLElement*)xmlNode->FirstChildElement("Stripes");
	if (!xmlElem1 || 0 != strcmp(xmlElem1->Value(), "Stripes"))
	{
		ResetData();
		AfxMessageBox(L"Can't find the <stripes> field to xml_file", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}
	/* stripe_count */
	xmlAttr = (tinyxml2::XMLAttribute*)xmlElem1->FirstAttribute();
	m_u8StripeCount = (UINT8)xmlAttr->IntValue();
	/* search for <Fiducials> */
	xmlElem1 = (tinyxml2::XMLElement*)xmlNode->FirstChildElement("Fiducials");
	if (!xmlElem1 || 0 != strcmp(xmlElem1->Value(), "Fiducials"))
	{
		ResetData();
		AfxMessageBox(L"Can't find the <fiducials> field to xml_file", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}
	/* search for <Global> */
	xmlElem2 = (tinyxml2::XMLElement*)xmlElem1->FirstChildElement("Global");
	if (!xmlElem2)
	{
		ResetData();
		AfxMessageBox(L"Can't find the <global> field to xml_file", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}
	/* search for <field> */
	xmlElem3 = (tinyxml2::XMLElement*)xmlElem2->FirstChildElement();
	for (stGbrXY.org_id = 0; xmlElem3; xmlElem3 = (tinyxml2::XMLElement*)xmlElem3->NextSiblingElement())
	{
		/* find the <org> */
		xmlData = (tinyxml2::XMLElement*)xmlElem3->FirstChildElement();
		xmlAttr = (tinyxml2::XMLAttribute*)xmlData->FirstAttribute();
		stOrgXY.mark_x = (DOUBLE)xmlAttr->DoubleValue();
		stOrgXY.mark_y = (DOUBLE)xmlAttr->Next()->DoubleValue();

		/* find the <bmp> */
		xmlData = (tinyxml2::XMLElement*)xmlData->NextSiblingElement();
		xmlAttr = (tinyxml2::XMLAttribute*)xmlData->FirstAttribute();
		stBmpXY.mark_x = (DOUBLE)xmlAttr->DoubleValue();
		stBmpXY.mark_y = (DOUBLE)xmlAttr->Next()->DoubleValue();

		/* find the <gbr> */
		xmlData = (tinyxml2::XMLElement*)xmlData->NextSiblingElement();
		xmlAttr = (tinyxml2::XMLAttribute*)xmlData->FirstAttribute();
		stGbrXY.mark_x = (DOUBLE)xmlAttr->DoubleValue();
		stGbrXY.mark_y = (DOUBLE)xmlAttr->Next()->DoubleValue();

		/* Add */
		lstGbrXY.AddTail(stGbrXY);	/* OrgID ���� Zero ���� ����Ǳ� ���� */
		stGbrXY.org_id++;
	}
	/* Global Mark�� �ּ� 2�� �̻��̾�� �� */
	if (lstGbrXY.GetCount() < 2 || lstGbrXY.GetCount() > 4 || align_type == ENG_ATGL::en_global_0_local_0x0_n_point)
	{
		lstGbrXY.RemoveAll();
		return TRUE;	/* ���� FALSE �� �ʿ� ����. */
	}
	if (lstGbrXY.GetCount() && lstGbrXY.GetCount())
	{
		/* Global Fiducial ���� �۾� ���� */
		if (!SortMarks(1, 1, UINT16(lstGbrXY.GetCount()), FALSE, lstGbrXY))
		{
			lstGbrXY.RemoveAll();
			res = FALSE;
		}
		/* ���� Global Fiducial�� ��ϵ� ������ŭ ���� */
		for (i = 0; i < lstGbrXY.GetCount(); i++)
		{
			stGbrXY = lstGbrXY.GetAt(lstGbrXY.FindIndex(i));
			stGbrXY.tgt_id = i;	/* !!! ���� �� ��ġ �� !!! (Zero-based) */
			m_pFidGlobal->AddData(stGbrXY);
		}
		/* Global Mark TYpe �� ���� */
		m_pFidGlobal->SetMarkType(m_u16MarkType);
		/* ���� ��ϵ� �ӽ� �޸� ���� */
		lstGbrXY.RemoveAll();
	}

	///* search for <Local Fiducials> */
	//if (align_type != ENG_ATGL::en_global_0_local_0x0_n_point &&
	//	align_type != ENG_ATGL::en_global_4_local_0x0_n_point &&
	//	align_type != ENG_ATGL::en_global_3_local_0x0_n_point &&
	//	align_type != ENG_ATGL::en_global_2_local_0x0_n_point)
	//{
		xmlElem1 = (tinyxml2::XMLElement*)xmlNode->FirstChildElement("Fiducials");
		if (!xmlElem1 || 0 != strcmp(xmlElem1->Value(), "Fiducials"))	return FALSE;
		/* search for <Local> */
		xmlElem2 = (tinyxml2::XMLElement*)xmlElem1->FirstChildElement("Local");
		if (xmlElem2)
		{
			/* search for <field> */
			xmlElem3 = (tinyxml2::XMLElement*)xmlElem2->FirstChildElement();
			for (stGbrXY.org_id = 0; xmlElem3; xmlElem3 = (tinyxml2::XMLElement*)xmlElem3->NextSiblingElement())
			{
				/* find the <org> */
				xmlData = (tinyxml2::XMLElement*)xmlElem3->FirstChildElement();
				xmlAttr = (tinyxml2::XMLAttribute*)xmlData->FirstAttribute();
				stOrgXY.mark_x = (DOUBLE)xmlAttr->DoubleValue();
				stOrgXY.mark_y = (DOUBLE)xmlAttr->Next()->DoubleValue();

				/* find the <bmp> */
				xmlData = (tinyxml2::XMLElement*)xmlData->NextSiblingElement();
				xmlAttr = (tinyxml2::XMLAttribute*)xmlData->FirstAttribute();
				stBmpXY.mark_x = (DOUBLE)xmlAttr->DoubleValue();
				stBmpXY.mark_y = (DOUBLE)xmlAttr->Next()->DoubleValue();

				/* find the <gbr> */
				xmlData = (tinyxml2::XMLElement*)xmlData->NextSiblingElement();
				xmlAttr = (tinyxml2::XMLAttribute*)xmlData->FirstAttribute();
				stGbrXY.mark_x = (DOUBLE)xmlAttr->DoubleValue();
				stGbrXY.mark_y = (DOUBLE)xmlAttr->Next()->DoubleValue();
				/* Add */
				lstGbrXY.AddTail(stGbrXY);
				stGbrXY.org_id++;	/* OrgID�� Zero ���� ����Ǳ� ���� */
			}

			if (lstGbrXY.GetCount())
			{
				/* Local Fiducial ���� �۾� ���� */
				// by sysandj : Preview�� ���� define ���� �ʾ������ sorting����.
				//abh1000 Local
				//if (FALSE == bNotDefined)
				//{
				if (!SortMarks(u16Col, u16Row, u16Cnt, bSharedType, lstGbrXY))	return FALSE;
				//}
				// by sysandj : Preview�� ���� define ���� �ʾ������ sorting����.

				/* ���� Local Fiducial�� ��ϵ� ������ŭ ���� */
				for (i = 0; i < lstGbrXY.GetCount(); i++)
				{
					stGbrXY = lstGbrXY.GetAt(lstGbrXY.FindIndex(i));
					stGbrXY.tgt_id = i;	/* !!! ���� �� ��ġ �� !!! (Zero-based) */
					m_pFidLocal->AddData(stGbrXY);
				}
				/* ���� ��ϵ� �ӽ� �޸� ���� */
				lstGbrXY.RemoveAll();
			}
		}
	/*}*/

	/* Search For <PanelData> */
	xmlElem1 = (tinyxml2::XMLElement*)xmlNode->FirstChildElement("PanelData");
	if (xmlElem1 && 0 == strcmp(xmlElem1->Value(), "PanelData"))
	{
		/* Search for <Dynamic PanelData> */
		xmlElem2 = (tinyxml2::XMLElement*)xmlElem1->FirstChildElement();
		for (i = 0; xmlElem2;
			(xmlElem2 = (tinyxml2::XMLElement*)xmlElem2->NextSiblingElement()) && (i < MAX_PANEL_STRING_COUNT),
			i++)
		{
			if (0 == strcmp(xmlElem2->Value(), "Dynamic"))
			{
				m_pstPanelData[i].field_cnt = 0x00;
				/* Attribute .. d-code, type, content, rotation, sizeX, sizeY, inverse */
				xmlAttr = (tinyxml2::XMLAttribute*)xmlElem2->FirstAttribute();
				m_pstPanelData[i].d_code = (UINT8)xmlAttr->IntValue();	xmlAttr = (tinyxml2::XMLAttribute*)xmlAttr->Next();
				m_pstPanelData[i].type = (UINT8)xmlAttr->IntValue();	xmlAttr = (tinyxml2::XMLAttribute*)xmlAttr->Next();
				m_pstPanelData[i].content = (UINT8)xmlAttr->IntValue();	xmlAttr = (tinyxml2::XMLAttribute*)xmlAttr->Next();
				m_pstPanelData[i].rotation = (UINT16)xmlAttr->IntValue();	xmlAttr = (tinyxml2::XMLAttribute*)xmlAttr->Next();
				m_pstPanelData[i].size_x = (UINT32)xmlAttr->IntValue();	xmlAttr = (tinyxml2::XMLAttribute*)xmlAttr->Next();
				m_pstPanelData[i].size_y = (UINT32)xmlAttr->IntValue();	xmlAttr = (tinyxml2::XMLAttribute*)xmlAttr->Next();
				m_pstPanelData[i].inverse = (UINT8)xmlAttr->IntValue();	xmlAttr = (tinyxml2::XMLAttribute*)xmlAttr->Next();

				/* search for <field> */
				xmlElem3 = (tinyxml2::XMLElement*)xmlElem2->FirstChildElement();
				/* field ������ �ִ��� ���� ��, ��ǥ ������ �ִ��� ���� Ȯ�� */
				if (xmlElem3 && 0 == strcmp(xmlElem3->Value(), "field"))
				{
					while (xmlElem3)
					{
						m_pstPanelData[i].field_cnt++;	/* field ������ �� � �ִ��� ī���� */
						xmlElem3 = (tinyxml2::XMLElement*)xmlElem3->NextSiblingElement();
					}
				}
			}
		}
	}

	/* Gerber Name ���� (Included Path) */
	sprintf_s(m_szJobName, MAX_GERBER_NAME, "%s", job_name);
	/* ���� ���õ� Mark Type �� ���� */
	if (align_type != ENG_ATGL::en_global_0_local_0x0_n_point)	m_enAlignType = align_type;

	/* search for <Photohead> */
	xmlElem1 = (tinyxml2::XMLElement*)xmlNode->FirstChildElement("NominalPhotoheadOffset");
	if (!xmlElem1)
	{
		AfxMessageBox(L"Can't find the <NominalPhotoheadOffset> field to xml_file", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}
	/* Search for <Photohead> */
	xmlElem2 = (tinyxml2::XMLElement*)xmlElem1->FirstChildElement("Photohead");
	if (!xmlElem2)
	{
		ResetData();
		AfxMessageBox(L"Can't find the <Photohead> field to xml_file", MB_ICONSTOP | MB_TOPMOST);
		return FALSE;
	}
	for (i = 0; xmlElem2; xmlElem2 = (tinyxml2::XMLElement*)xmlElem2->NextSiblingElement(), i++)
	{
		/* find the <nominal_photohead_offset> */
		xmlData = (tinyxml2::XMLElement*)xmlElem2->FirstChildElement();
		/* find the <nominal_photohead_distance> */
		xmlData = (tinyxml2::XMLElement*)xmlData->NextSiblingElement();
		xmlAttr = (tinyxml2::XMLAttribute*)xmlData->FirstAttribute();
		/* �⺻ Offset �� ���� */
		m_pPhDistX[i] = xmlAttr->IntValue();
	}

	return res;
}

/*
 desc : Global Fiducial ��ǥ �� ���
 parm : index	- [in]  ����Ǿ� �ִ� Mark Number (Zero based)
		data	- [out] Gerber XY & Motion XY Position (unit: mm)
 retn : TRUE or FALSE
*/
BOOL CJobSelectXml::GetGlobalMark(UINT16 index, STG_XMXY& data)
{
	/* ��� �� ��ȯ */
	if (!m_pFidGlobal)	return FALSE;
	return m_pFidGlobal->GetMark(index, data);
}

CFiducialData* CJobSelectXml::GetGlobalMark()
{
	/* ��� �� ��ȯ */
	if (!m_pFidGlobal)	return nullptr;
	return m_pFidGlobal;
}

CFiducialData* CJobSelectXml::GetLocalMark()
{
	/* ��� �� ��ȯ */
	if (!m_pFidLocal)	return nullptr;
	return m_pFidLocal;
}


/*
 desc : Global Fiducial ��ǥ �� ���� ��ġ �� ���
 parm : direct	- [in]  �� ���� ��ǥ�� ������ϴ� ���� ���� ���� ��
		data1	- [out] Gerber XY & Motion XY Position (unit: mm)
		data2	- [out] Gerber XY & Motion XY Position (unit: mm)
 retn : TRUE or FALSE
*/
BOOL CJobSelectXml::GetGlobalMark(ENG_GMDD direct, STG_XMXY& data1, STG_XMXY& data2)
{
	/* ��� �� ��ȯ */
	if (!m_pFidGlobal)	return FALSE;
	/* ��ũ ������ �������� ���� Ȯ�� */
	if (m_pFidGlobal->GetCount() < 4)	return FALSE;

	/* ���� �Ź��� ��ǥ ���� ������ nm(����)�̹Ƿ�, 100���� ������� 100 nm or 0.1 um�� �ȴ�.*/
	switch (direct)
	{
	case ENG_GMDD::en_top_horz: if (!m_pFidGlobal->GetMark(0, data1))	return 0;
		if (!m_pFidGlobal->GetMark(2, data2))	return 0;	break;
	case ENG_GMDD::en_btm_horz: if (!m_pFidGlobal->GetMark(1, data1))	return 0;
		if (!m_pFidGlobal->GetMark(3, data2))	return 0;	break;
	case ENG_GMDD::en_lft_vert: if (!m_pFidGlobal->GetMark(0, data1))	return 0;
		if (!m_pFidGlobal->GetMark(1, data2))	return 0;	break;
	case ENG_GMDD::en_rgt_vert: if (!m_pFidGlobal->GetMark(2, data1))	return 0;
		if (!m_pFidGlobal->GetMark(3, data2))	return 0;	break;
	case ENG_GMDD::en_lft_diag: if (!m_pFidGlobal->GetMark(0, data1))	return 0;
		if (!m_pFidGlobal->GetMark(3, data2))	return 0;	break;
	case ENG_GMDD::en_rgt_diag: if (!m_pFidGlobal->GetMark(1, data1))	return 0;
		if (!m_pFidGlobal->GetMark(2, data2))	return 0;	break;
	}

	return TRUE;
}

/*
 desc : Local Fiducial ��ǥ �� ���
 parm : index	- [in]  ����Ǿ� �ִ� ��ġ (Zero based)
		data	- [out] Gerber XY & Motion XY Position (unit: mm)
 retn : TRUE or FALSE
*/
BOOL CJobSelectXml::GetLocalMark(UINT16 index, STG_XMXY& data)
{
	/* ��� �� ��ȯ */
	if (!m_pFidLocal)	return FALSE;
	return m_pFidLocal->GetMark(index, data);
}

/*
 desc : Local Fiducal ��ġ ������ ����� �޸� Index (Zero-based) �� ��ȯ
 parm : scan	- [in]  ��ĵ�Ϸ��� ��ġ (��ȣ. 0 or 1)
		cam_id	- [in]  Align Camera Index (1 or 2)
 retn : Zero-based Index �� ��ȯ
*/
UINT8 CJobSelectXml::GetLocalBottomMark(UINT8 scan, UINT8 cam_id)
{
	UINT8 u8Index = 0x00;

	//if (!m_pFidLocal || cam_id < 1 || cam_id > 2 || scan > 1)	return 0xff;



	if (!m_pFidLocal || std::clamp((int)cam_id, 1, 2) != cam_id)	return 0xff; //���� ��ĵ������ ������Ѵ�. 

	auto status = alignMotion->status;

	int centerCol = status.GetCenterColumn();

	u8Index = cam_id == 1 ? status.markMapConst[scan].back().tgt_id : status.markMapConst[centerCol + scan].back().tgt_id;

	//
	//	switch (m_enAlignType)
	//	{
	//		/* Normal Partition */
	//	case ENG_ATGL::en_global_4_local_2x1_n_point:	/* Global (4) points / Local Division (2 x 1) (08) points */
	//		if (cam_id == 1)	u8Index = 3;				/* Local Division�� 2 x 1�� ���, Scan ���� ������ 0�̾�� �� */
	//		else				u8Index = 7;
	//		break;
	//	case ENG_ATGL::en_global_4_local_2x2_n_point:	/* Global (4) points / Local Division (2 x 2) (16) points */
	//		//abh1000(0308)
	//		//if (0 == scan)		u8Index	= 3;
	//		//else				u8Index	= 11;
	//		//if (cam_id == 2)	u8Index	= u8Index + 4;
	//		if (0 == scan)		u8Index = 3;
	//		else				u8Index = 7;
	//		if (cam_id == 2)	u8Index = u8Index + 8;
	//		break;
	//	case ENG_ATGL::en_global_4_local_3x2_n_point:	/* Global (4) points / Local Division (3 x 2) (24) points */
	//		//abh1000 0417
	//// 		if (0 == scan)		u8Index	= 5;
	//// 		else				u8Index	= 17;
	//// 		if (cam_id == 2)	u8Index	= u8Index + 6;
	//		if (0 == scan)		u8Index = 5;
	//		else				u8Index = 11;
	//		if (cam_id == 2)	u8Index = u8Index + 12;
	//		break;
	//	case ENG_ATGL::en_global_4_local_4x2_n_point:	/* Global (4) points / Local Division (4 x 2) (32) points */
	//		// 		if (0 == scan)		u8Index	= 7;
	//		// 		else				u8Index	= 23;
	//		// 		if (cam_id == 2)	u8Index	= u8Index + 8;
	//		if (0 == scan)		u8Index = 7;
	//		else				u8Index = 15;
	//		if (cam_id == 2)	u8Index = u8Index + 16;
	//		break;
	//	case ENG_ATGL::en_global_4_local_5x2_n_point:	/* Global (4) points / Local Division (5 x 2) (40) points */
	//		// 		if (0 == scan)		u8Index	= 9;
	//		// 		else				u8Index	= 29;
	//		// 		if (cam_id == 2)	u8Index	= u8Index + 10;
	//		if (0 == scan)		u8Index = 9;
	//		else				u8Index = 19;
	//		if (cam_id == 2)	u8Index = u8Index + 20;
	//		break;
	//		/* Shared Partition */
	//	case ENG_ATGL::en_global_4_local_2x2_s_point:	/* Global (4) points / Local Division (2 x 2) (13) points */
	//		if (0 == scan)		u8Index = 2;
	//		else				u8Index = 5;
	//		if (cam_id == 2)	u8Index = u8Index + 3;
	//		break;
	//	case ENG_ATGL::en_global_4_local_3x2_s_point:	/* Global (4) points / Local Division (3 x 2) (16) points */
	//		if (0 == scan)		u8Index = 3;
	//		else				u8Index = 7;
	//		if (cam_id == 2)	u8Index = u8Index + 4;
	//		break;
	//	case ENG_ATGL::en_global_4_local_4x2_s_point:	/* Global (4) points / Local Division (4 x 2) (19) points */
	//		if (0 == scan)		u8Index = 4;
	//		else				u8Index = 9;
	//		if (cam_id == 2)	u8Index = u8Index + 5;
	//		break;
	//	case ENG_ATGL::en_global_4_local_5x2_s_point:	/* Global (4) points / Local Division (5 x 2) (22) points */
	//		if (0 == scan)		u8Index = 5;
	//		else				u8Index = 11;
	//		if (cam_id == 2)	u8Index = u8Index + 6;
	//		break;
	//	}

	return u8Index;
}

/*
 desc : Local Fiducial ��ǥ �� ��� - Left / Bottom
 parm : scan	- [in]  ��ĵ�Ϸ��� ��ġ (��ȣ. 0 or 1)
		cam_id	- [in]  Align Camera Index (1 or 2)
		data	- [out] Fiducial ���� ����� ���� ����
 retn : TRUE or FALSE
*/
BOOL CJobSelectXml::GetLocalBottomMark(UINT8 scan, UINT8 cam_id, STG_XMXY& data)
{
	UINT8 u8Index = 0x00;

	//if (!m_pFidLocal || cam_id < 1 || cam_id > 2 || scan > 1)	return FALSE;

	//if (!m_pFidLocal || cam_id < 1 || cam_id > 2 || scan > 1)	return FALSE;
	if (!m_pFidLocal || std::clamp((int)cam_id, 1, 2) != cam_id)	return 0xff; //���� ��ĵ������ ������Ѵ�. 


	/* Left or Right / Bottom Index �� ��� */
	u8Index = GetLocalBottomMark(scan, cam_id);
	if (0xff == u8Index)	return FALSE;

	/* �� ���� X ���� ���� ������ ���� ���ϱ� (���� ��) */
	if (!m_pFidLocal->GetMark(u8Index, data))	return FALSE;

	return TRUE;
}

/*
 desc : �Ź��� ��ϵ� Global Fiducial ���� ��ȯ
 parm : mark	- [in]  Mark Type (ENG_AMTF)
 retn : ����
*/
UINT8 CJobSelectXml::GetMarkCount(ENG_AMTF mark)
{
	if (mark == ENG_AMTF::en_global)
	{
		if (!m_pFidGlobal)	return 0;
		return (UINT8)m_pFidGlobal->GetCount();
	}
	else if (mark == ENG_AMTF::en_local)
	{
		if (!m_pFidLocal)	return 0;
		return (UINT8)m_pFidLocal->GetCount();
	}
	else
	{
		if (!m_pFidGlobal || !m_pFidLocal)	return 0;
		return (UINT8)(m_pFidGlobal->GetCount() + m_pFidLocal->GetCount());
	}
}

/*
 desc : 1 Scan �������� ����� Local Mark ���� ��ȯ
 parm : None
 retn : ����
*/

//Ÿ����ġ.
UINT8 CJobSelectXml::GetLocalMarkCountPerScan()
{
	UINT8 u8Count = alignMotion->status.gerberRowCnt;// GlobalVariables::GetInstance()->GetAlignMotion().status.gerberRowCnt;
	return u8Count;


	//switch (m_enAlignType)
	//{
	///* Normal Partition */
	//case ENG_ATGL::en_global_4_local_2x1_n_point : u8Count	= 4;	break;	/* Global (4) points / Local Division (2 x 2) (16) points */
	//case ENG_ATGL::en_global_4_local_2x2_n_point : u8Count	= 4;	break;	/* Global (4) points / Local Division (2 x 2) (16) points */
	//case ENG_ATGL::en_global_4_local_3x2_n_point : u8Count	= 6;	break;	/* Global (4) points / Local Division (3 x 2) (24) points */
	//case ENG_ATGL::en_global_4_local_4x2_n_point : u8Count	= 8;	break;	/* Global (4) points / Local Division (4 x 2) (32) points */
	//case ENG_ATGL::en_global_4_local_5x2_n_point : u8Count	= 10;	break;	/* Global (4) points / Local Division (5 x 2) (40) points */
	///* Shared Partition */						   
	//case ENG_ATGL::en_global_4_local_2x2_s_point : u8Count	= 3;	break;	/* Global (4) points / Local Division (2 x 2) (13) points */
	//case ENG_ATGL::en_global_4_local_3x2_s_point : u8Count	= 4;	break;	/* Global (4) points / Local Division (3 x 2) (16) points */
	//case ENG_ATGL::en_global_4_local_4x2_s_point : u8Count	= 5;	break;	/* Global (4) points / Local Division (4 x 2) (19) points */
	//case ENG_ATGL::en_global_4_local_5x2_s_point : u8Count	= 6;	break;	/* Global (4) points / Local Division (5 x 2) (22) points */
	//}

	//return u8Count;
}

/*
 desc : Job Name ��
 parm : job_name	- [in]  ���ϰ��� �ϴ� Job Name�� ����� ���� ������
 retn : TRUE (����), FALSE (�ٸ���)
*/
BOOL CJobSelectXml::IsCompareJobName(CHAR* job_name)
{
	return 0 == strcmp(job_name, m_szJobName) ? TRUE : FALSE;
}

/*
 desc : �Ź� ������ ���� Local�� Global Mark ��� ��ϵǾ� �ִ��� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CJobSelectXml::IsMarkMixedGlobalLocal()
{
	if (!m_pFidGlobal || !m_pFidLocal)	return FALSE;
	return m_pFidGlobal->GetCount() > 0 && m_pFidLocal->GetCount() > 0;
}

/*
 desc : �Ź� ������ ���� Global Mark ��ϵǾ� �ִ��� ����
 parm : None
 retn : TRUE or FALSE
*/
BOOL CJobSelectXml::IsMarkExistGlobal()
{
	if (!m_pFidGlobal)	return FALSE;
	return m_pFidGlobal->GetCount() > 0;
}

/*
 desc : ���� ������ : X �� �������� 2 ���� ��ũ�� ������ ��ġ ��ȯ (���� : mm)
 parm : direct	- [in]  ���� ��ũ ���� (0 - 1�� ��ũ�� 3�� ��ũ ���� ���� ��)
									   (1 - 2�� ��ũ�� 4�� ��ũ ���� ���� ��)
									   (2 - 1�� ��ũ�� 2�� ��ũ ���� ���� ��)
									   (3 - 3�� ��ũ�� 4�� ��ũ ���� ���� ��)
									   (4 - 1�� ��ũ�� 4�� ��ũ ���� ���� ��)
									   (5 - 2�� ��ũ�� 3�� ��ũ ���� ���� ��)
 retn : X ���� 2���� ��ũ ������ ���� (����: mm) ��ȯ
*/
DOUBLE CJobSelectXml::GetGlobalMarkDist(ENG_GMDD direct)
{
	STG_XMXY stData[2] = { STG_XMXY(),};

	if (!GetGlobalMark(direct, stData[0], stData[1]))	return 0.0f;
	/* ���� �� ��ȯ */
	return sqrt(pow(stData[0].mark_x - stData[1].mark_x, 2) + pow(stData[0].mark_y - stData[1].mark_y, 2));
}

/*
 desc : �� Mark ���� ��/�� X �� ������ ���� ��, X �� ���� ���� (�Ÿ�) ��
 parm : mark_x1	- [in]  X1 �� Mark Number (1-based. 0x01 ~ 0x04)
		mark_x2	- [in]  X2 �� Mark Number (1-based. 0x01 ~ 0x04)
 retn : �� ������ Y �� ���� ���� ���� (����: mm)
*/
DOUBLE CJobSelectXml::GetGlobalMarkDiffX(UINT8 mark_x1, UINT8 mark_x2)
{
	if (!m_pFidGlobal)	return 0;

	UINT8 u8Mark = (UINT8)m_pFidGlobal->GetCount();
	STG_XMXY stData[2] = { STG_XMXY(),};

	if ((u8Mark != 4) && (u8Mark != 2) && (u8Mark != 3))	return 0;
	if (mark_x1 < 1 || mark_x2 < 1 || mark_x1 > 4 || mark_x2 > 4 || mark_x1 == mark_x2)	return 0;

	/* �ش� ��ġ�� ���� X/Y ��ǥ �� ��� */
	if (!m_pFidGlobal->GetMark(mark_x1 - 1, stData[0]))	return 0;
	if (!m_pFidGlobal->GetMark(mark_x2 - 1, stData[1]))	return 0;

	/* �� Y ��ǥ�� ������ ���� �� ��ȯ */
	return (stData[0].mark_x - stData[1].mark_x);
}

/*
 desc : �� Mark ���� ��/�� Y �� ������ ���� ��, Y �� ���� ���� (����) ��
 parm : mark_y1	- [in]  Y1 �� Mark Number (1-based. 0x01 ~ 0x04)
		mark_y2	- [in]  Y2 �� Mark Number (1-based. 0x01 ~ 0x04)
 retn : �� ������ Y �� ���� ���� ���� (����: mm)
*/
DOUBLE CJobSelectXml::GetGlobalMarkDiffY(UINT8 mark_y1, UINT8 mark_y2)
{
	if (!m_pFidGlobal)	return 0;

	UINT8 u8Mark = (UINT8)m_pFidGlobal->GetCount();
	STG_XMXY stData[2] = { STG_XMXY(), };

	if ((u8Mark != 4) && (u8Mark != 2) && (u8Mark != 3))	return 0;
	if (mark_y1 < 1 || mark_y2 < 1 || mark_y1 > 4 || mark_y2 > 4 || mark_y1 == mark_y2)	return 0;

	/* �ش� ��ġ�� ���� X/Y ��ǥ �� ��� */
	if (!m_pFidGlobal->GetMark(mark_y1 - 1, stData[0]))	return 0;
	if (!m_pFidGlobal->GetMark(mark_y2 - 1, stData[1]))	return 0;

	/* �� Y ��ǥ�� ������ ���� �� ��ȯ */
	return (stData[0].mark_y - stData[1].mark_y);
}

/*
 desc : Mark 1���� 3��, Mark 2���� 4���� X ��ǥ ���� ���� ��ȯ (���� ���� ��)
 parm : type	- [in]  0 : Mark 2�� �������� Mark 1���� X ���� �� ��ȯ
						1 : Mark 4�� �������� Mark 3���� X ���� �� ��ȯ
						2 : Mark 1�� �������� Mark 2���� X ���� �� ��ȯ
						3 : Mark 3�� �������� Mark 4���� X ���� �� ��ȯ
 retn : �� ��ũ�� X ��ġ ���� ���� �� ��ȯ (����: mm) ��ȯ
*/
DOUBLE CJobSelectXml::GetGlobalMarkDiffVertX(UINT8 type)
{
	if (!m_pFidGlobal)	return 0;

	UINT8 u8Mark = (UINT8)m_pFidGlobal->GetCount();
	STG_XMXY stData[2] = { STG_XMXY(), };

#if 0
	/* ��ũ ������ �������� ���� Ȯ�� */
	if (m_pFidGlobal->GetCount() < 4)	return 0;
#else
	if ((u8Mark != 4) && (u8Mark != 2) && (u8Mark != 3))	return 0.0f;
#endif
	if (0 == type)
	{
		if (!m_pFidGlobal->GetMark(1, stData[0]))	return 0;
		if (!m_pFidGlobal->GetMark(0, stData[1]))	return 0;
	}
	else if (0x01 == type)
	{
		if (!m_pFidGlobal->GetMark(3, stData[0]))	return 0;
		if (!m_pFidGlobal->GetMark(2, stData[1]))	return 0;
	}
	else if (0x02 == type)
	{
		if (!m_pFidGlobal->GetMark(0, stData[0]))	return 0;
		if (!m_pFidGlobal->GetMark(1, stData[1]))	return 0;
	}
	else /*if (0x03 == type)*/
	{
		if (!m_pFidGlobal->GetMark(2, stData[0]))	return 0;
		if (!m_pFidGlobal->GetMark(3, stData[1]))	return 0;
	}
	/* �� ��ȯ */
	return (stData[0].mark_x - stData[1].mark_x);
}



/*
 desc : Align Camera 1���� 2�� ���� Mark X �� ������ ����
 parm : mode	- [in]  0x00 : Width, 0x01 : Height
		scan	- [in]  ��ĵ�Ϸ��� ��ġ (��ȣ. 0 or 1)
 retn : �� ������ X �� ���� ������ �Ÿ� (����: mm)
*/
DOUBLE CJobSelectXml::GetLocalMarkACam12DistX(UINT8 mode, UINT8 scan)
{
	UINT8 u8CamX1 = 0x00, u8CamX2 = 0x00;
	DOUBLE dbDiff = 0.0f;
	STG_XMXY stData[2] = { STG_XMXY(), };

	//if (scan > 1)	return 0; ���� ��ĵ������ �������.


	auto status = alignMotion->status;

	
	int centerCol = status.GetCenterColumn();

	u8CamX1 = status.markMapConst[scan].back().tgt_id; //1ķ
	u8CamX2 = status.markMapConst[centerCol + scan].back().tgt_id; //2ķ


	//switch (m_enAlignType)
	//{
	//	/* Normal Partition */
	//case ENG_ATGL::en_global_4_local_2x1_n_point:	/* Global (4) points / Local Division (2 x 1) (08) points */
	//	u8CamX1 = 3;	/* 1 x 2 ����� ��ũ �迭 ������ Scan�� 1 �� �ۿ� �����Ƿ�, scan ���� �ǹ̴� ����*/
	//	u8CamX2 = 7;
	//	break;
	//case ENG_ATGL::en_global_4_local_2x2_n_point:	/* Global (4) points / Local Division (2 x 2) (16) points */
	//	//abh1000 0417
	//	// 		if (0 == scan)	u8CamX1	= 3;
	//	// 		else			u8CamX1	= 11;
	//	// 		u8CamX2	= u8CamX1 + 4;
	//	if (0 == scan)	u8CamX1 = 3;
	//	else			u8CamX1 = 7;
	//	u8CamX2 = u8CamX1 + 8;
	//	break;
	//case ENG_ATGL::en_global_4_local_3x2_n_point:	/* Global (4) points / Local Division (3 x 2) (24) points */
	//	// 		if (0 == scan)	u8CamX1	= 5;
	//	// 		else			u8CamX1	= 17;
	//	// 		u8CamX2	= u8CamX1 + 6;
	//	if (0 == scan)	u8CamX1 = 5;
	//	else			u8CamX1 = 11;
	//	u8CamX2 = u8CamX1 + 12;
	//	break;
	//case ENG_ATGL::en_global_4_local_4x2_n_point:	/* Global (4) points / Local Division (4 x 2) (32) points */
	//	// 		if (0 == scan)	u8CamX1	= 7;
	//	// 		else			u8CamX1	= 23;
	//	// 		u8CamX2	= u8CamX1 + 8;
	//	if (0 == scan)	u8CamX1 = 7;
	//	else			u8CamX1 = 15;
	//	u8CamX2 = u8CamX1 + 16;
	//	break;
	//case ENG_ATGL::en_global_4_local_5x2_n_point:	/* Global (4) points / Local Division (5 x 2) (40) points */
	//	// 		if (0 == scan)	u8CamX1	= 9;
	//	// 		else			u8CamX1	= 29;
	//	// 		u8CamX2	= u8CamX1 + 10;
	//	if (0 == scan)	u8CamX1 = 9;
	//	else			u8CamX1 = 19;
	//	u8CamX2 = u8CamX1 + 20;
	//	break;
	//	/* Shared Partition */
	//case ENG_ATGL::en_global_4_local_2x2_s_point:	/* Global (4) points / Local Division (2 x 2) (13) points */
	//	if (0 == scan)	u8CamX1 = 2;
	//	else			u8CamX1 = 5;
	//	u8CamX2 = u8CamX1 + 3;
	//	break;
	//case ENG_ATGL::en_global_4_local_3x2_s_point:	/* Global (4) points / Local Division (3 x 2) (16) points */
	//	if (0 == scan)	u8CamX1 = 3;
	//	else			u8CamX1 = 7;
	//	u8CamX2 = u8CamX1 + 4;
	//	break;
	//case ENG_ATGL::en_global_4_local_4x2_s_point:	/* Global (4) points / Local Division (4 x 2) (19) points */
	//	if (0 == scan)	u8CamX1 = 4;
	//	else			u8CamX1 = 9;
	//	u8CamX2 = u8CamX1 + 5;
	//	break;
	//case ENG_ATGL::en_global_4_local_5x2_s_point:	/* Global (4) points / Local Division (5 x 2) (22) points */
	//	if (0 == scan)	u8CamX1 = 5;
	//	else			u8CamX1 = 11;
	//	u8CamX2 = u8CamX1 + 6;
	//	break;
	//}

	/* ��ũ ������ �������� ���� Ȯ�� */
	if (m_pFidLocal->GetCount() <= u8CamX2)			return 0;
	/* �� ���� X ���� ���� ������ ���� ���ϱ� (���� ��) */
	if (!m_pFidLocal->GetMark(u8CamX1, stData[0]))	return 0;
	if (!m_pFidLocal->GetMark(u8CamX2, stData[1]))	return 0;

	/* ���� �� ��ȯ (mm�� 100 nm or 0.1 um ������ ��ȯ�ϱ� ���� 10000 ���ϱ�) */
	if (0x00 == mode)	dbDiff = abs(stData[0].mark_x - stData[1].mark_x);
	else 				dbDiff = abs(stData[0].mark_y - stData[1].mark_y);

	return dbDiff;
}

/*
 desc : �� Mark ���� ��/�� Y �� ������ ���� ��, Y �� ���� ���� (����) ��
 parm : mark_y1	- [in]  Y1 �� Mark Number (Zero based)
		mark_y2	- [in]  Y2 �� Mark Number (Zero based)
 retn : �� ������ Y �� ���� ���� ���� (����: mm)
*/
DOUBLE CJobSelectXml::GetLocalMarkDiffY(UINT8 mark_y1, UINT8 mark_y2)
{
	STG_XMXY stData[2] = { STG_XMXY(), };

	if (m_pFidLocal->GetCount() <= mark_y1 ||
		m_pFidLocal->GetCount() <= mark_y2)	return 0;

	/* �ش� ��ġ�� ���� X/Y ��ǥ �� ��� */
	if (!m_pFidLocal->GetMark(mark_y1, stData[0]))	return 0;
	if (!m_pFidLocal->GetMark(mark_y2, stData[1]))	return 0;

	/* �� Y ��ǥ�� ������ ���� �� ��ȯ */
	return (stData[0].mark_y - stData[1].mark_y);
}

/*
 desc : Scan �������� Left.Top�� Right.Top�� X ��ǥ ��������, �� ������ X ��ǥ���� �󸶳� �Ź� �󿡼�
		������ �ְ� ��ġ�Ǿ� �ִ��� �� ���� ������ ���ϱ� ����
 parm : scan	- [in]  Align Scan�Ϸ��� ��ġ (��ȣ. 0 or 1)
		cam1	- [in]  Left/Bottom�� ��ġ�� Mark X ��ġ �������� ���� �� ��ȯ (Left/Top ������ ����) (����: mm)
		cam2	- [in]  Right/Bottom�� ��ġ�� Mark X ��ġ �������� ���� �� ��ȯ (Right/Top ������ ����) (����: mm)
 retn : ����� ������ ���� ��ȯ (������ ���, 0)
*/
UINT32 CJobSelectXml::GetLocalMarkDiffVertX(UINT8 scan,
	CAtlList <DOUBLE>& cam1, CAtlList <DOUBLE>& cam2)
{
	UINT8 i, u8Count = 0x00, u8CamX1 = 0x00, u8CamX2 = 0x00;
	STG_XMXY stTgt[2] = { STG_XMXY(), }, stOrg[2] = { STG_XMXY(), };

	//if (scan > 1)	return 0; //���� ��ĵ���� ������Ѵ�. 


	auto status = alignMotion->status;

	int cols = status.gerberColCnt;

	if (cols % 2 != 0)
	{
		LOG_ERROR(ENG_EDIC::en_luria, L"column must be devide by 2");
		return 0;
	}

	int centerCol = cols / status.acamCount;

	auto cam1Scan = status.markMapConst[scan];
	auto cam2Scan = status.markMapConst[centerCol + scan];

	if (cam1Scan.size() != cam2Scan.size())
	{
		LOG_ERROR(ENG_EDIC::en_luria, L"scan count must be same");
		return 0;
	}


	u8CamX1 = cam1Scan.front().tgt_id; //1ķ
	u8CamX2 = cam2Scan.front().tgt_id; //2ķ


	u8Count = cam1Scan.size();

	///* Normal Partition */
	//switch (m_enAlignType)
	//{
	//case ENG_ATGL::en_global_4_local_2x1_n_point	:	/* Global (4) points / Local Division (2 x 1) (08) points */
	//	u8CamX1	= 0;	/* Scan�� 1�� �ۿ� �����Ƿ�... */
	//	u8Count	= 4;
	//	break;
	//case ENG_ATGL::en_global_4_local_2x2_n_point	:	/* Global (4) points / Local Division (2 x 2) (16) points */
	//	if (0 == scan)	u8CamX1	= 0;
	//	else			u8CamX1	= 8;
	//	u8Count	= 4;
	//	break;
	//case ENG_ATGL::en_global_4_local_3x2_n_point	:	/* Global (4) points / Local Division (3 x 2) (24) points */
	//	if (0 == scan)	u8CamX1	= 0;
	//	else			u8CamX1	= 12;
	//	u8Count	= 6;
	//	break;
	//case ENG_ATGL::en_global_4_local_4x2_n_point	:	/* Global (4) points / Local Division (4 x 2) (32) points */
	//	if (0 == scan)	u8CamX1	= 0;
	//	else			u8CamX1	= 16;
	//	u8Count	= 8;
	//	break;
	//case ENG_ATGL::en_global_4_local_5x2_n_point	:	/* Global (4) points / Local Division (5 x 2) (40) points */
	//	if (0 == scan)	u8CamX1	= 0;
	//	else			u8CamX1	= 20;
	//	u8Count	= 10;
	//	break;
	///* Shared Partition */
	//case ENG_ATGL::en_global_4_local_2x2_s_point	:	/* Global (4) points / Local Division (2 x 2) (13) points */
	//	if (0 == scan)	u8CamX1	= 0;
	//	else			u8CamX1	= 3;
	//	u8Count	= 3;
	//	break;
	//case ENG_ATGL::en_global_4_local_3x2_s_point	:	/* Global (4) points / Local Division (3 x 2) (16) points */
	//	if (0 == scan)	u8CamX1	= 0;
	//	else			u8CamX1	= 4;
	//	u8Count	= 4;
	//	break;
	//case ENG_ATGL::en_global_4_local_4x2_s_point	:	/* Global (4) points / Local Division (4 x 2) (19) points */
	//	if (0 == scan)	u8CamX1	= 0;
	//	else			u8CamX1	= 5;
	//	u8Count	= 5;
	//	break;
	//case ENG_ATGL::en_global_4_local_5x2_s_point	:	/* Global (4) points / Local Division (5 x 2) (22) points */
	//	if (0 == scan)	u8CamX1	= 0;
	//	else			u8CamX1	= 6;
	//	u8Count	= 6;
	//	break;
	//}
	/* Align Camera 2���� Y ��ǥ ���� ���� ���� ��ġ */
	//u8CamX2 = u8CamX1 + u8Count;

	/* ��ũ ������ �������� ���� Ȯ�� */
	if (1 > u8Count)
	{
		LOG_ERROR(ENG_EDIC::en_luria, L"Not enough local marks (1 > marks)");
		return 0;
	}

	//if (0x00 == scan)
	//{
	//	if (!m_pFidLocal->GetMark(u8CamX1 + u8Count - 1, stOrg[0]) ||
	//		!m_pFidLocal->GetMark(u8CamX2 + u8Count - 1, stOrg[1]))
	//	{
	//		LOG_ERROR(ENG_EDIC::en_luria, L"Failed to get the mark of Left or Bottom");
	//		return 0;
	//	}
	//}
	//else
	//{
		if (!m_pFidLocal->GetMark(u8CamX1, stOrg[0]) ||
			!m_pFidLocal->GetMark(u8CamX2, stOrg[1]))
		{
			LOG_ERROR(ENG_EDIC::en_luria, L"Failed to get the mark of Left or Bottom");
			return 0;
		}
	//}

	/* Scan �������� ���� X (Left.Top�� Right.Top�� X ��ǥ) �������� ��ŭ ���������� ���� */
	for (i = u8Count; i > 0; i--)
	{
		/* ù ��° �ε��� ���� ���������� �����ؼ� �� ī�޶��� Left / Bottom���� ���� �� ��ȯ */
		if (!m_pFidLocal->GetMark(cam1Scan[i - 1].tgt_id, stTgt[0]) ||
			!m_pFidLocal->GetMark(cam2Scan[i - 1].tgt_id, stTgt[1]))
		{
			cam1.RemoveAll();
			cam2.RemoveAll();
			LOG_ERROR(ENG_EDIC::en_luria, L"Failed to get the mark of Left or Bottom");
			return 0;
		}
		cam1.AddTail(stTgt[0].mark_x - stOrg[0].mark_x);
		cam2.AddTail(stTgt[1].mark_x - stOrg[1].mark_x);
	}

	return (UINT32)cam1.GetCount();
}

/*
 desc : Mark Type ��������, Camera 1���� Left/Bottom�� Camera 2���� Right/Bottom�� Y ��ǥ ���� ���� �� ��ȯ
 parm : None
 retn : �� ������ Y �� ���� ���� ���� (����: mm)
*/
DOUBLE CJobSelectXml::GetGlobalLeftRightBottomDiffY()
{
	UINT16 u16Index[2] = { 1, 3 };	/* �⺻ Global Mark�� 4���� ��� */
	STG_XMXY stData[2] = { STG_XMXY(), };

	if (!m_pFidGlobal)	return 0;
	/* Global Mark�� 2 or 4���� ���ο� ���� */
	if (m_pFidGlobal->GetCount() == 2)
	{
		u16Index[0] = 0;	/* Mark 2���� ���, Left ���� 1�� ��ũ */
		u16Index[1] = 1;	/* Mark 2���� ���, Right ���� 2�� ��ũ */
	}
	if (!m_pFidGlobal->GetMark(u16Index[0], stData[0]))	return 0;
	if (!m_pFidGlobal->GetMark(u16Index[1], stData[1]))	return 0;

	return (stData[0].mark_y - stData[1].mark_y);
}

/*
 desc : Global Fiducial�� Left/Bottom or Right/Bottom Mark�� Local Fiducial�� ���� ��ġ�� �ش�Ǵ�
		���� ���� �� ��, ������ ���� ��
 parm : direct	- [in]  Global Fiducial�� ��ġ ����. 0x00 : Left/Bottom, 0x01 : Right/Bottom
		index	- [in]  Local Fiducial�� ��ġ ������ ����� �ε��� (Zero-based)
 retn : �� ��ũ ���� ������ ���� �� (����: mm)
*/
DOUBLE CJobSelectXml::GetGlobalBaseMarkLocalDiffY(UINT8 direct, UINT8 index)
{
	BOOL bSucc = FALSE;
	STG_XMXY stGlobal = { STG_XMXY(), }, stLocal = { STG_XMXY(), };;
	double absTolerance = 1;



	if (!m_pFidGlobal)	return 0;
	/* Global Fiducial�� Base Mark�� Left/Bottom�� �Ź� ��ġ ���� */
	if (0x00 == direct)	bSucc = m_pFidGlobal->GetMark(1, stGlobal);
	//if (0x00 == direct)	bSucc = m_pFidGlobal->GetMark(0, stGlobal);
	else				bSucc = m_pFidGlobal->GetMark(3, stGlobal);
	//else				bSucc = m_pFidGlobal->GetMark(4, stGlobal);
	if (!bSucc)	return 0;
	/* Local Fiducial�� ���� ��ġ ���� ��� */
	bSucc = m_pFidLocal->GetMark(index, stLocal);
	if (!bSucc)	return 0;

	double diff = ((stLocal.mark_y * 10000.0f) - (stGlobal.mark_y * 10000.0f));

	/* 2 ��ũ ���� ���� ���� �� ��ȯ */ 
	return (abs(diff) <= absTolerance) ? 0.0f : diff / 10000;//���� ��ǥ�� 2���� �����Ұ�쿡 �ԽǷ��̿��ؾ��Ѵ�. 
}

/*
 desc : ��ũ ���� ������ ����, �Էµ� ��ũ ��ġ(��ȣ)�� �����⿡ ���ϴ��� �����⿡ ���ϴ��� Ȯ��
 parm : mark_no	- [in]  Align Mark Number (0 or Later)
 retn : TRUE (Forward) or FALSE (Backward)
*/
//BOOL CJobSelectXml::IsMarkDirectForward(UINT8 mark_no)
//{
//	STG_DBXY stData[2]	= {NULL};
//	
//	���� �̰� ���� ���Ű��ݾ�.
//
//	/* Mark Number�� 1 �����̸� ������ ������ */
//	if (mark_no < 2)	return TRUE;
//	switch (m_enAlignType)
//	{
//	/* Normal or Shared Partition */
//	case ENG_ATGL::en_global_4_local_2x1_n_point	:	/* Global (4) points / Local Division (2 x 1) (08) points */
//	case ENG_ATGL::en_global_4_local_2x2_n_point	:	/* Global (4) points / Local Division (2 x 2) (16) points */
//	case ENG_ATGL::en_global_4_local_2x2_s_point	:	/* Global (4) points / Local Division (3 x 2) (24) points */
//		if (mark_no < 6)	return FALSE;	break;
//	case ENG_ATGL::en_global_4_local_3x2_n_point	:	/* Global (4) points / Local Division (3 x 2) (24) points */
//	case ENG_ATGL::en_global_4_local_3x2_s_point	:	/* Global (4) points / Local Division (3 x 2) (24) points */
//		if (mark_no < 8)	return FALSE;	break;
//	case ENG_ATGL::en_global_4_local_4x2_n_point	:	/* Global (4) points / Local Division (4 x 2) (32) points */
//	case ENG_ATGL::en_global_4_local_4x2_s_point	:	/* Global (4) points / Local Division (4 x 2) (32) points */
//		if (mark_no < 10)	return FALSE;	break;
//	case ENG_ATGL::en_global_4_local_5x2_n_point	:	/* Global (4) points / Local Division (5 x 2) (40) points */
//	case ENG_ATGL::en_global_4_local_5x2_s_point	:	/* Global (4) points / Local Division (5 x 2) (40) points */
//		if (mark_no < 12)	return FALSE;	break;
//	}
//
//	return TRUE;
//}

/*
 desc : �Ź� ũ�� ��ȯ
 parm : width	- [out] ���� �� ��ȯ (����: mm)
		height	- [out] ���� �� ��ȯ (����: mm)
 retn : None
*/
VOID CJobSelectXml::GetGerberSize(DOUBLE& width, DOUBLE& height)
{
	width = m_dbGerberWidth;
	height = m_dbGerberHeight;
}

/*
 desc : Serial String (Serial D-Code)�� ���� Field ���� ��ȯ
 parm : None
 retn : 0 (�˻��� Serial D-Code ���� ���ų� Field�� ���� ���) or Later
*/
UINT16 CJobSelectXml::GetSerialDCodeFieldCount()
{
	UINT16 i = 0, u16Count = 0;

	do {

		/* ��ȿ�� ���� �ִ��� Ȯ��. ��, �Էµ� ������ �ʵ����� ���� */
		if (!m_pstPanelData)			break;
		if (!m_pstPanelData[i].d_code)	break;
		/* Serial DCode ������ ��츸 �ش�� */
		if (0 == m_pstPanelData[i].content && m_pstPanelData[i].field_cnt > 0)
		{
			u16Count += m_pstPanelData[i].field_cnt;
		}
		i++;

	} while (i < MAX_PANEL_STRING_COUNT);

	return u16Count;
}