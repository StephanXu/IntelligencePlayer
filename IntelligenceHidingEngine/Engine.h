#pragma once
#include <vector>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <ctime>
#include <Windows.h>
#include <sstream>
#include <iterator>
using namespace std;

#define RND(a,b) (rand() % (b-a+1))+ a
#define PLAYER_OFFSET(current,off) ((current + abs(off > 0 ? off : (2 * off))) % 3)
#define CARDS_JUDGE_GET_BASE_SCORE(cnum) (cnum-10)

#define C_J 11
#define C_Q 12
#define C_K 13
#define C_A 14
#define C_2 15
#define C_SJ 16
#define C_LJ 17

#define LANDLOARD 0
using namespace std;

class CCardGroup
{
public:
	CCardGroup()
		:m_Type(0)
		, m_Weight(0)
	{

	}
	multiset<int> m_cardroot;
	multiset<int> m_cardbroght;

	/*
	Type parts(each part for two bytes)
	Type:	[each_root_long][root_count] [bring_root_max][bring_count]
	Weight:	[root_min]
	Example:	Cards		Type		Weight
	3334		03010101	03
	33344456	03020102	03
	*/
	unsigned int m_Type;
	unsigned int m_Weight;
	int m_Score;
	inline void SetType(unsigned char each_root_long, unsigned char root_count, unsigned char bring_root_max, unsigned char bring_count)
	{
		((unsigned char*)&m_Type)[0] = bring_count;
		((unsigned char*)&m_Type)[1] = bring_root_max;
		((unsigned char*)&m_Type)[2] = root_count;
		((unsigned char*)&m_Type)[3] = each_root_long;
	}

	inline int GetEachRootLong()
	{
		return (int)(((unsigned char*)&m_Type)[3]);
	}

	inline int GetRootCount()
	{
		return (int)(((unsigned char*)&m_Type)[2]);
	}

	inline int GetBringRootMax()
	{
		return (int)(((unsigned char*)&m_Type)[1]);
	}

	inline int GetBringCount()
	{
		return (int)(((unsigned char*)&m_Type)[0]);
	}

	inline void CombineCards(vector<int>& Dest)
	{
		Dest.assign(m_cardroot.begin(), m_cardroot.end());
		Dest.insert(Dest.end(), m_cardbroght.begin(), m_cardbroght.end());
		/*for (auto i = m_cardroot.begin(); i != m_cardroot.end(); ++i)
		{
			Dest.push_back(*i);
		}
		for (auto i = m_cardbroght.begin(); i != m_cardbroght.end(); ++i)
		{
			Dest.push_back(*i);
		}*/
	}

	inline void CombineCards(multiset<int>& Dest)
	{
		Dest.insert(m_cardroot.begin(), m_cardroot.end());
		Dest.insert(m_cardbroght.begin(), m_cardbroght.end());
		/*for (auto i = m_cardroot.begin(); i != m_cardroot.end(); ++i)
		{
			Dest.insert(*i);
		}
		for (auto i = m_cardbroght.begin(); i != m_cardbroght.end(); ++i)
		{
			Dest.insert(*i);
		}*/
	}

	inline int AnalyseCardsSet(vector<int> Source)
	{
		if (Source.empty())
			return 0;//empty source
		if (Source == vector<int>{16, 17})
		{
			m_cardroot = multiset<int>{ 16,17 };
			m_cardbroght.clear();
			m_Weight = 16;
			SetType(4, 1, 0, 0);
			return 1;
		}
		m_cardroot.clear();
		m_cardbroght.clear();
		Source.push_back(0);//to expend loop
		int min{ *Source.begin() };
		int each_root_len{};
		int each_bring_len{};
		int root_count{};
		int bring_count{};
		multiset<int> Line;
		int line_num{ *Source.begin() };
		for (auto it{ Source.begin() }; it != Source.end(); ++it)
		{
			if (line_num != *it)
			{
				if (!each_root_len)
					each_root_len = (int)Line.size();
				if (Line.size() >= (unsigned int)each_root_len)
				{
					m_cardroot.insert(Line.begin(), Line.end());
					root_count++;
				}
				else
				{
					m_cardbroght.insert(Line.begin(), Line.end());
					bring_count++;
				}
				Line.clear();
				line_num = *it;
			}
			if (*it == 0)
				break;
			Line.insert(*it);
		}

		m_Weight = *Source.begin();
		if (bring_count)
		{
			bring_count = root_count;
			each_bring_len = (unsigned int)m_cardbroght.size() / bring_count;
		}
		SetType(
			each_root_len,
			(unsigned char)(m_cardroot.size() / each_root_len),
			each_bring_len,
			each_bring_len > 0 ? (unsigned char)(m_cardbroght.size() / each_bring_len) : 0);
		return 1;
	}

	inline bool MatchType(unsigned int type)
	{
		return type == m_Type;
	}

	inline int Compare(CCardGroup& cg)
	{
		if (MatchType(cg.m_Type))
		{
			return m_Weight - cg.m_Weight;
		}
		else
		{
			if (cg.GetEachRootLong() == 4 && cg.GetBringCount() == 0 && GetEachRootLong() < 4)
			{
				return -2;
			}
			if (cg.GetEachRootLong() == 4 && cg.GetBringCount() == 0 && GetEachRootLong() == 4 && GetBringCount() == 0)
			{
				return m_Weight - cg.m_Weight;
			}

			if (GetEachRootLong() == 4 && GetBringCount() == 0 && cg.GetEachRootLong() < 4)
			{
				return 2;
			}
		}
		return -1;
	}

	inline int CalcScore()
	{
		m_Score = CARDS_JUDGE_GET_BASE_SCORE(*--m_cardroot.end());
		/*if (GetBringCount())
		{
			int i{ 1 };
			for (auto it = m_cardbroght.begin();
				it != m_cardbroght.end(); ++it, i++)
			{
				if (i%GetBringRootMax() == 0)
				{
					m_Score += CARDS_JUDGE_GET_BASE_SCORE(*it);
				}
			}
		}*/
		if (GetEachRootLong() == 4)
			m_Score += 18;
		if (((GetEachRootLong() == 2 || GetEachRootLong() == 3) && GetRootCount() == 1 && *--m_cardroot.end() == 15) ||
			(GetEachRootLong() == 1 && GetRootCount() >= 5 && *--m_cardroot.end() == 14))
			m_Score += 7;
		
		return m_Score;
	}
};

class CCardMatrix
{
public:
#define STATISTICS_SINGLE 0
#define STATISTICS_DOUBLE 1
#define STATISTICS_THREE 2
#define STATISTICS_FOUR 3

	unsigned char m_CardStatistics[15][4]{};

	inline void FillMatrix(multiset<int>& cards)
	{
		for (auto it = cards.begin(); it != cards.end(); ++it)
		{
			m_CardStatistics[*it - 3][STATISTICS_SINGLE]++;
			for (int i{ STATISTICS_DOUBLE }; i <= STATISTICS_FOUR; ++i)
			{
				if (m_CardStatistics[*it - 3][STATISTICS_SINGLE] % (i + 1) == 0)
					m_CardStatistics[*it - 3][i] = 1;
			}
		}
	}

	inline void PrintMatrix(string& PrintStr)
	{
		PrintStr.clear();
		for (int x{}; x < 15; x++)
		{
			for (int y{}; y < 4; y++)
			{
				char buf[0x10]{};
				_itoa_s((int)m_CardStatistics[x][y], buf, 10);
				PrintStr += buf;
				PrintStr += '\t';
			}
			PrintStr += '\n';
		}
	}

	inline void Clear()
	{
		ZeroMemory(m_CardStatistics, 15 * 4);
	}
};

class CCardDivider
{
public:
	void Divide(vector<CCardGroup*>& Destination, multiset<int>& cards, CCardGroup& lastcardgroup, bool enable_last)
	{
		//vector<CCardGroup*> groups;
		groups.clear();
		//CCardMatrix matrix;
		matrix_divider.Clear();
		//CCardGroup lcg = lastcardgroup;
		multiset<int> Line[4]{};
		multiset<int> NewGroupCards;//move to here to avoid wasting resources

		matrix_divider.FillMatrix(cards);
		for (int i{}; i < 15; i++)
		{
			for (int li{ (/*enable_last ? (lcg.GetEachRootLong() - 1) :*/ STATISTICS_SINGLE) };
				li <= (/*enable_last ? (lcg.GetEachRootLong() - 1) :*/ STATISTICS_FOUR);
				li++)
			{
				if (matrix_divider.m_CardStatistics[i][li] > 0 &&
					(li == STATISTICS_FOUR || matrix_divider.m_CardStatistics[i][STATISTICS_FOUR] == 0) &&//to avoid dividing bomb
					(i < 12 || Line[li].empty()))//to avoid get '2' into line struct and keep '2' brings
				{
					for (int insert_i{}; insert_i < li + 1; insert_i++)
						Line[li].insert(i + 3);
					//add default(without brings)
					CCardGroup* pNewGroup = new CCardGroup;
					//multiset<int> NewGroupCards;
					NewGroupCards.clear();
					for (int new_cards_i{}; new_cards_i < li + 1; new_cards_i++)
						NewGroupCards.insert(i + 3);
					pNewGroup->m_cardroot = NewGroupCards;
					pNewGroup->m_Weight = *NewGroupCards.begin();
					pNewGroup->SetType(li + 1, 1, 0, 0);
					pNewGroup->CalcScore();
					groups.push_back(pNewGroup);
				}
				else
				{
					for (; Line[li].size();)
					{
						for (int erase_i{}; erase_i < li + 1; erase_i++)
						{
							//if (Line[li].begin() != Line[li].end())
							if (!Line[li].empty())
								Line[li].erase(Line[li].begin());
							else
								break;
						}
						AnalyseBrings(groups, cards, Line[li], li);
					}
				}

				//add brings
				AnalyseBrings(groups, cards, Line[li], li);
			}
		}

		if (matrix_divider.m_CardStatistics[13][STATISTICS_SINGLE] > 0 &&
			matrix_divider.m_CardStatistics[14][STATISTICS_SINGLE] > 0)
		{
			CCardGroup* pNewGroup = new CCardGroup;
			//multiset<int> BoombCards{ 16,17 };
			pNewGroup->m_cardroot = BoombCards;
			pNewGroup->m_Weight = *BoombCards.begin();
			pNewGroup->SetType(4, 1, 0, 0);
			pNewGroup->CalcScore();
			groups.push_back(pNewGroup);
		}

		if (enable_last)
		{
			for (auto it = groups.begin(); it != groups.end(); ++it)
			{
				/*vector<int> v;
				(*it)->CombineCards(v);
				for (auto a = v.begin(); a != v.end(); a++)
				{
				cout << *a;
				}
				cout << endl;
				if ((*it)->m_cardroot == multiset<int>{5, 5, 5, 5} && (*it)->GetBringCount() == 0)
				cout << "ok2" << endl;*/
				if ((*it)->Compare(lastcardgroup) > 0)
				{
					Destination.push_back(*it);
				}
				else
				{
					delete *it;
				}
			}
		}
		else
			Destination.insert(Destination.begin(), groups.begin(), groups.end());
	}

	void Divide(vector<CCardGroup*>& Destination, multiset<int>& cards, const vector<int>* lastcards)
	{
		CCardGroup lcg;
		if (lastcards)
			if (!lcg.AnalyseCardsSet(*lastcards))lastcards = 0;
		Divide(Destination, cards, lcg, lastcards != 0);
	}

	static char Num2CardName(int num)
	{
		if (num == 3)
			return '3';
		if (num == 4)
			return '4';
		if (num == 5)
			return '5';
		if (num == 6)
			return '6';
		if (num == 7)
			return '7';
		if (num == 8)
			return '8';
		if (num == 9)
			return '9';
		if (num == 10)
			return 'X';
		if (num == 11)
			return 'J';
		if (num == 12)
			return 'Q';
		if (num == 13)
			return 'K';
		if (num == 14)
			return 'A';
		if (num == 15)
			return '2';
		if (num == 16)
			return 'b';
		if (num == 17)
			return 'B';
		return 0;
	}

	static int CardName2Num(char card_name)
	{
		if (card_name == '3')
			return 3;
		if (card_name == '4')
			return 4;
		if (card_name == '5')
			return 5;
		if (card_name == '6')
			return 6;
		if (card_name == '7')
			return 7;
		if (card_name == '8')
			return 8;
		if (card_name == '9')
			return 9;
		if (card_name == 'X')
			return 10;
		if (card_name == 'J')
			return 11;
		if (card_name == 'Q')
			return 12;
		if (card_name == 'K')
			return 13;
		if (card_name == 'A')
			return 14;
		if (card_name == '2')
			return 15;
		if (card_name == 'b')
			return 16;
		if (card_name == 'B')
			return 17;

		return 0;
	}
private:
	//whole
	void GetBrings(
		unordered_multimap<int, multiset<int>>& Destination,
		multiset<int>&pool,
		int EachBringLength,
		int EachBringMaxLength,
		int BringNum)
	{
		//CCardMatrix matrix;
		matrix_brings.Clear();
		if (EachBringLength != 0) EachBringLength--;

		matrix_brings.FillMatrix(pool);
		multiset<int> types;
		vector<multiset<int>> type_group;
		multiset<int> types_ins;
		for (int li{ (EachBringLength == 0 ? STATISTICS_SINGLE : EachBringLength) };
			li <= (EachBringLength == 0 ? EachBringMaxLength - 1 : EachBringLength); li++)
		{
			types.clear();
			type_group.clear();
			for (int i{}; i < 13/*15*/; i++)//to keep joker not to be brought
			{
				if (matrix_divider.m_CardStatistics[i][STATISTICS_FOUR] > 0)//to keep bomb not to be brought
					continue;
				if (matrix_brings.m_CardStatistics[i][li] > 0)
				{
					types.insert(i + 3);
				}
			}
			if (!types.empty())
			{
				Comb(types, type_group, BringNum);
				for (auto it{ type_group.begin() }; it != type_group.end(); ++it)
				{
					types_ins.clear();
					for (int i{}; i <= li; ++i)
						copy(it->begin(), it->end(), inserter(types_ins, --types_ins.end()));

					Destination.insert(pair<int, multiset<int>>(li + 1, types_ins));
				}
			}
		}

		return;
	}
	//default(line brings)
	void GetBrings_Less(
		unordered_multimap<int, multiset<int>>& Destination,
		multiset<int>&pool,
		int EachBringLength,
		int EachBringMaxLength,
		int BringNum)
	{
		//CCardMatrix matrix;
		matrix_brings.Clear();
		if (EachBringLength != 0) EachBringLength--;

		matrix_brings.FillMatrix(pool);
		multiset<int> Line[4]{};

		for (int i{}; i < 13/*15*/; i++)//to keep joker not to be brought
		{
			for (int li{ (EachBringLength == 0 ? STATISTICS_SINGLE : EachBringLength) };
				li <= (EachBringLength == 0 ? EachBringMaxLength - 1 : EachBringLength); li++)
			{
				if (matrix_divider.m_CardStatistics[i][STATISTICS_FOUR] > 0)//to keep bomb not to be brought
					continue;
				if (matrix_brings.m_CardStatistics[i][li] > 0)
				{
					for (int insert_i{}; insert_i < li + 1; insert_i++)
						Line[li].insert(i + 3);
				}
				else
					Line[li].clear();
				if (Line[li].size() / (li + 1) >= (unsigned int)BringNum)
				{
					Destination.insert(pair<int, multiset<int>>(li + 1, Line[li]));
					for (int insert_i{}; insert_i < li + 1; insert_i++)
						Line[li].erase(Line[li].begin());
				}
			}
		}
	}

	void AnalyseBrings(
		vector<CCardGroup*>&DestCatResult,
		multiset<int> cards_pool,
		const multiset<int>& cards_root,
		int root_type)
	{
		static const unsigned int ShortestLine[4]{ 5,3,1,5 };//the forth situation we ignored the brings(it makes big mistakes). it used to be "1"
		unsigned int root_count{ (unsigned int)(cards_root.size() / (root_type + 1)) };
		if ((root_count >= ShortestLine[root_type]) &&
			(root_type != STATISTICS_SINGLE || *--cards_root.end() < 15) &&
			(root_type != STATISTICS_FOUR || (cards_root.size() / (root_type + 1)) == ShortestLine[STATISTICS_FOUR]) &&
			(root_count < 2 || *--cards_root.end() < 15))
		{
			if (root_type >= STATISTICS_THREE) //consider bringing
			{
				for (auto it = cards_root.begin(); it != cards_root.end(); ++it)
				{
					auto find_result{ cards_pool.find(*it) };
					cards_pool.erase(find_result);
				}
				//unordered_multimap<int, multiset<int>> all_brings;
				all_brings.clear();
				GetBrings(
					all_brings,
					cards_pool,
					0,
					2,
					(int)(cards_root.size() / (root_type + 1))*(root_type - STATISTICS_THREE + 1));
				for (auto brings_it{ all_brings.begin() }; brings_it != all_brings.end(); ++brings_it)
				{
					unsigned int bring_count{ (unsigned int)brings_it->second.size() / brings_it->first };
					if (root_type == STATISTICS_THREE &&
						brings_it->first == 1 &&
						bring_count == 1 &&
						*--cards_root.end() == *brings_it->second.begin())
					{
						continue;
					}
					CCardGroup* pNewGroup = new CCardGroup;
					multiset<int> NewGroupCards;
					pNewGroup->m_cardroot = cards_root;
					pNewGroup->m_cardbroght = brings_it->second;
					pNewGroup->m_Weight = *cards_root.begin();
					pNewGroup->SetType(
						root_type + 1,
						(unsigned char)root_count,
						brings_it->first,
						(unsigned char)bring_count);
					pNewGroup->CalcScore();
					DestCatResult.push_back(pNewGroup);
					//cout << "ok" << endl;
				}
			}
			if (root_count > 1)
			{
				//have no brings(334455 and 34567)
				CCardGroup* pNewGroup = new CCardGroup;
				multiset<int> NewGroupCards;
				pNewGroup->m_cardroot = cards_root;
				pNewGroup->m_Weight = *cards_root.begin();
				pNewGroup->SetType(
					root_type + 1,
					(unsigned char)cards_root.size() / (root_type + 1),
					0,
					0);
				pNewGroup->CalcScore();
				DestCatResult.push_back(pNewGroup);
			}
		}

	}

	//elenum:num in each group
	//sourcenum:sources' num
	template<typename _T>
	void Comb(vector<_T>& Source, vector<vector<_T>>& Dest, int elenum)
	{
		int sourcenum = (unsigned int)Source.size();
		if (sourcenum < elenum)
			return;
		char* index = new char[sourcenum];
		memset(index, 1, elenum);
		memset(index + elenum, 0, sourcenum - elenum);
		int num1{};
		vector<_T> sit;
		sit.clear();
		for (int pr{}; pr < sourcenum; pr++)
		{
			if (index[pr])
				sit.push_back(Source[pr]);
		}
		Dest.push_back(sit);
		for (int i{}; i < sourcenum - 1; ++i)
		{
			if (*(short*)(index + i) == 0x1)//0001 0000
			{
				*(short*)(index + i) = 0x100;//0000 0001
				memset(index, 1, num1);
				memset(index + num1, 0, i - num1);
				num1 = 0;
				i = -1;
				sit.clear();
				for (int pr{}; pr < sourcenum; pr++)
				{
					if (index[pr])
						sit.push_back(Source[pr]);
				}
				Dest.push_back(sit);
				continue;
			}
			if (index[i] == 1)
				num1++;
		}
		delete[] index;
	}

	template<typename _T>
	void Comb(multiset<_T>& Source, vector<multiset<_T>>& Dest, int elenum)
	{
		vector<_T> vsource;
		copy(Source.begin(), Source.end(), back_inserter(vsource));

		int sourcenum = (unsigned int)vsource.size();
		if (sourcenum < elenum)
			return;

		char* index = new char[sourcenum];
		memset(index, 1, elenum);
		memset(index + elenum, 0, sourcenum - elenum);
		int num1{};
		multiset<_T> sit;
		sit.clear();
		for (int pr{}; pr < sourcenum; pr++)
		{
			if (index[pr])
				sit.insert(vsource[pr]);
		}
		Dest.push_back(sit);
		for (int i{}; i < sourcenum - 1; ++i)
		{
			if (*(short*)(index + i) == 0x1)//0001 0000
			{
				*(short*)(index + i) = 0x100;//0000 0001
				memset(index, 1, num1);
				memset(index + num1, 0, i - num1);
				num1 = 0;
				i = -1;
				sit.clear();
				for (int pr{}; pr < sourcenum; pr++)
				{
					if (index[pr])
						sit.insert(vsource[pr]);
				}
				Dest.push_back(sit);
				continue;
			}
			if (index[i] == 1)
				num1++;
		}
		delete[] index;
	}

	unordered_multimap<int, multiset<int>> all_brings;
	vector<CCardGroup*> groups;
	CCardMatrix matrix_divider; // the matrix for function:divide
	CCardMatrix matrix_brings; // the matrix for GetBrings
	const multiset<int> BoombCards{ 16,17 };
};

class CCardsJudge
{
public:

	double GetCardsPoint(multiset<int>& cards)
	{
		if (cards.empty())
			return 40;
#define SCORE_SCALE 25
		vector<CCardGroup*> all_group;
		m_div.Divide(all_group, cards, 0);
		double avr_length{};
		double avr_score{};
		double final_score{};
		for (auto it{ all_group.begin() }; it != all_group.end(); ++it)
		{
			avr_length += (*it)->m_cardroot.size() + (*it)->m_cardbroght.size();
			avr_score += (*it)->m_Score;
			delete *it;
		}
		avr_length /= (double)all_group.size()*cards.size();
		avr_score /= (double)all_group.size();
		final_score += (m_complex_prop*avr_length + m_base_prop*avr_score);
		return final_score;
	}

	void GetPath(multiset<int> cards, vector<CCardGroup*>& current, vector<vector<CCardGroup*>>& path)
	{
		vector<CCardGroup*> groups;
		m_div.Divide(groups, cards, 0);
		vector<int> gcpc;

		if (groups.empty())
		{
			path.push_back(current);
			return;
		}

		for (auto it{ groups.begin() }; it != groups.end(); ++it)
		{
			gcpc.clear();
			(*it)->CombineCards(gcpc);
			for (auto itd{ gcpc.begin() }; itd != gcpc.end(); ++itd)
			{
				auto fs = cards.find(*itd);
				if (fs != cards.end())
					cards.erase(fs);
			}
			current.push_back(*it);
			GetPath(cards, current, path);
			current.pop_back();
		}

		return;
	}

	CCardDivider m_div;

	static double m_complex_prop;
	static double m_base_prop;
};

double CCardsJudge::m_complex_prop = 0;

double CCardsJudge::m_base_prop = 0;

class CPutCard
{
public:
	CCardGroup m_cards;
	bool m_IsPass;

	bool operator ==(CPutCard& S)
	{
		return (m_IsPass == S.m_IsPass) && (m_cards.m_cardroot == S.m_cards.m_cardroot) && (m_cards.m_cardbroght == S.m_cards.m_cardbroght);
		/*if (m_IsPass == S.m_IsPass && S.m_cards.Compare(m_cards) == 0)
		{
		return true;
		}
		return false;*/
	}
};

class CPlayer
{
public:
	multiset<int> m_Cards;
	CPutCard m_LastPut;
	void PutCards(CPutCard& output)
	{
		if (output.m_cards.m_cardroot.size() == 0)
		{
			output.m_IsPass = true;
			m_LastPut = output;
			//cout << "pass";
			return;
		}

		multiset<int> pc;
		output.m_cards.CombineCards(pc);
		for (auto it = pc.begin(); it != pc.end(); ++it)
		{
			auto put_card_in_cards{ m_Cards.find(*it) };
			if (put_card_in_cards != m_Cards.end())
			{
				//cout << *put_card_in_cards << ",";
				m_Cards.erase(put_card_in_cards);
			}
		}
		m_LastPut = output;
		return;
	}
};

class CTableStatus
{
public:
	CTableStatus()
		:m_CurrentPlayer(0)

	{
		m_player[0].m_LastPut.m_IsPass = true;
		m_player[1].m_LastPut.m_IsPass = true;
		m_player[2].m_LastPut.m_IsPass = true;
	}
	CPlayer m_player[3];
	int m_CurrentPlayer;
	//return -1 for not end. others for winner's id
	inline int JudgeWinner()
	{
		for (int i{ 0 }; i < 3; i++)
		{
			if (m_player[i].m_Cards.size() == 0)
			{
				return i;
			}
		}
		return -1;
	}

	int GiveOutCards()
	{
		//
		vector<int> card_pool;
		for (int i{ 3 }; i <= C_2; i++)
		{
			for (int i2{ 0 }; i2 < 4; i2++)
			{
				card_pool.push_back(i);
			}
		}
		card_pool.push_back(C_SJ);
		card_pool.push_back(C_LJ);
		random_shuffle(card_pool.begin(), card_pool.end());

		auto it = card_pool.begin();

		for (int ip{ 0 }; ip < 3; ip++)
		{
			for (int i{ 0 }; i < 17; i++)
			{

				m_player[ip].m_Cards.insert(*it);
				it++;
			}
		}

		m_player[0].m_Cards.insert(*it);
		it++;
		m_player[0].m_Cards.insert(*it);
		it++;
		m_player[0].m_Cards.insert(*it);

		return 0;
	}

	inline void ChangePlayer()
	{
		if (++m_CurrentPlayer > 2)
			m_CurrentPlayer = 0;
	}

	inline CPlayer* LastPlayer()
	{
		if (m_CurrentPlayer == 0)
			return &m_player[2];
		return &m_player[m_CurrentPlayer - 1];
	}

	inline CPlayer* LastNotPassPlayer()
	{
		int player_id = m_CurrentPlayer;
		for (--player_id < 0 ? player_id = 2 : player_id; player_id != m_CurrentPlayer; --player_id < 0 ? player_id = 2 : player_id)
		{
			if (!m_player[player_id].m_LastPut.m_IsPass)
				return &m_player[player_id];
		}
		return 0;
	}

	inline void PutCard(CPutCard cards)
	{
		m_player[m_CurrentPlayer].PutCards(cards);
		ChangePlayer();
	}
};

class CNode
{
public:
	~CNode()
	{
		for (auto it{ m_Children.begin() }; it != m_Children.end(); ++it)
		{
			if (*it) delete *it;
		}
	}
	CPutCard m_Action;
	vector<CNode*> m_Children;
	int m_ChildrenDepth;
	long long m_CountVisit;
	long long m_CountWin;
	
	int m_IntegralNode;
	int m_IntegralWinNode;
	bool m_IntegralMarked;

	long double m_ChildrnWinrateSum;
	long double m_ChildrnWinrateSum_Squ;
	double m_BasicScore;

	CNode* m_BestNext;
	CNode* m_Dad;
};

class CSimulation
{
public:
#define uctK 1
	CSimulation()
		:m_SimuationCount(0)
		,m_WinCount(0)
		,m_Root(nullptr)
		,RandomResult(-1)
		,Depth(-1)
		,BombDis(-1)
	{

	}
	CTableStatus m_CurrentStatus;
	int m_SimuationCount;
	int m_WinCount;
	CNode* m_Root;

	int RandomResult;
	int Depth;

	//last bomb distance
	int BombDis;

	CCardsJudge m_Judge;

	void Initialize(CTableStatus& root_status,CNode* root)
	{
		m_CurrentStatus = root_status;
		m_SimuationCount = 0;
		m_WinCount = 0;
		m_Root = root;
		return;
	}

	void SetStatus(CTableStatus& status)
	{
		//status = m_CurrentStatus;
		m_CurrentStatus = status;
	}

	//Calculate the weight of UCTSelection
	//CNode* node: the node of the step(usually the root's children)
	//could be override
	virtual inline double UctSelectionCalcWeight(CNode* node, CNode* dad_node)
	{
		double win_rate{ (double)node->m_CountWin / (double)node->m_CountVisit };
		/*double uct = uctK * sqrt(log(node->m_CountVisit) / (5 * node->m_CountVisit));
		return
		m_CurrentStatus.m_CurrentPlayer > 0 ?
		win_rate + uct :
		1 - win_rate + uct;*/
		double uct = win_rate + 1 * sqrt(log(dad_node->m_CountVisit) / log(node->m_CountVisit));
		return uct;
	}

	//To get the UCT value and get the node to have simulation
	virtual CNode* UCTSelection(CNode* root)
	{
		CNode* Chosen{ nullptr };
		double MaxUct{ -1000 };

		for (auto it = root->m_Children.begin();
			it != root->m_Children.end();
			++it)
		{
			double val{};
			if ((*it)->m_CountVisit > 0)
			{
				val = UctSelectionCalcWeight((*it), root);
			}
			else
			{
				val = 10000 + rand() % 1000;
			}
			if (val > MaxUct)
			{
				MaxUct = val;
				Chosen = (*it);
			}
		}
		return Chosen;
	}

	//To convergence scope factor of the UCT formula
	virtual inline void ConvergenceUCTScopeFactor(double decay_factor)
	{
		return;
	}

	//Calculate the win_rate
	//CNode* node: the node of the step(usually the root's children)
	//Could be override
	virtual inline double WinRateCalc(CNode* node)
	{
		return (double)node->m_CountWin / (double)node->m_CountVisit;
	}

	//Get the best choice of the next step
	//CNode& root: the node of current root
	CNode* GetBestWay(CNode& root)
	{
		double MaxWinRate{ -1000 };
		CNode* MaxWinRateNode{ nullptr };

		bool IntegralNode{ false };

		for (auto it{ root.m_Children.begin() }; it != root.m_Children.end(); ++it)
		{
			if (IntegralNode)
			{
				if ((*it)->m_Action.m_cards.GetEachRootLong() != 4)
					MaxWinRateNode = *it;
				continue;
			}
			else if ((*it)->m_IntegralNode > 0 && (*it)->m_IntegralWinNode == (*it)->m_IntegralNode)
			{
				MaxWinRateNode = *it;
				IntegralNode = true;
				continue;
			}

			double win_rate{ WinRateCalc((*it)) };
			if (MaxWinRate < win_rate)
			{
				if ((*it)->m_Action.m_cards.GetEachRootLong() != 4 || win_rate >= 0.7)
				{
					MaxWinRate = win_rate;
					MaxWinRateNode = *it;
				}
			}
			else if (MaxWinRate == win_rate)
			{
				int current_len = (*it)->m_Action.m_cards.GetEachRootLong()*(*it)->m_Action.m_cards.GetRootCount() +
					(*it)->m_Action.m_cards.GetBringRootMax()*(*it)->m_Action.m_cards.GetBringCount();
				int exsist_len = MaxWinRateNode->m_Action.m_cards.GetEachRootLong()*MaxWinRateNode->m_Action.m_cards.GetRootCount() +
					MaxWinRateNode->m_Action.m_cards.GetBringRootMax()*MaxWinRateNode->m_Action.m_cards.GetBringCount();
				if (current_len > exsist_len)
				{
					MaxWinRate = win_rate;
					MaxWinRateNode = *it;
				}
			}

		}

		return MaxWinRateNode;
	}

	//return random game which started with 'status' reuslt. could be override
	virtual int RandomGame(CTableStatus status)
	{

		return 0;
	}

	//enum all possibilities of current status
	//CNode* node: the node need to create children node
	virtual int EnumPossibilities(CNode* node, CTableStatus current_status)
	{

		return 0;
	}

	virtual bool IsRefreshNodeWinCount(int current_id, int game_result)
	{
		if (current_id == 1)
		{
			if (game_result == 0)
			{
				return true;
			}
		}
		else
		{
			if (game_result > 0)
			{
				return true;
			}
		}
		return false;
	}

	int Simulation(CNode* root)
	{
		int CurrentStatusPlayerId = m_CurrentStatus.m_CurrentPlayer;

		int result = m_CurrentStatus.JudgeWinner();
		if (-1 != result)
		{
			RandomResult = result;
			if (IsRefreshNodeWinCount(CurrentStatusPlayerId, RandomResult))
				m_WinCount++;
			m_SimuationCount++;
			Depth = 0;
		}
		else if (root->m_CountVisit == 0)
		{
			RandomResult = RandomGame(m_CurrentStatus);
			if (IsRefreshNodeWinCount(CurrentStatusPlayerId, RandomResult))
				m_WinCount++;
			//root->m_BasicScore = m_Judge.GetCardsPoint(m_CurrentStatus.m_player[CurrentStatusPlayerId].m_Cards);
			Depth = 0;
		}
		else
		{
			//prepare filter argument
			if (BombDis != -1)
			{
				if (++BombDis > 6)
					BombDis = -1;
			}
			if (root->m_Action.m_cards.GetEachRootLong() == 4)
				BombDis = 1;

			if (root->m_Children.size() == 0)
			{
				EnumPossibilities(root, m_CurrentStatus);
				//if (root == m_Root && root->m_Children.size() == 1)
				//{
				//	if (root->m_Children[0]->m_Action.m_IsPass = true)
				//	{
				//		root->m_BestNext = root->m_Children[0];
				//		return -1;//break out
				//	}
				//}
			}
			CNode* next_node{ UCTSelection(root) };

			m_CurrentStatus.PutCard(next_node->m_Action);
			Simulation(next_node);
		}

		root->m_ChildrenDepth = Depth++;
		root->m_CountVisit++;

		if (result != -1)
		{
			if (root->m_Dad && !root->m_IntegralMarked)
			{
				if (result == 0)
					root->m_Dad->m_IntegralWinNode++;
				root->m_Dad->m_IntegralNode++;
				root->m_IntegralMarked = true;
			}
		}
		else
		{
			if (root->m_Dad &&
				!root->m_Children.empty() &&
				//root->m_IntegralNode > 0 &&
				(root->m_IntegralNode == (int)root->m_Children.size()) &&
				!root->m_IntegralMarked)
			{
				root->m_Dad->m_IntegralNode++;
				if (root->m_IntegralWinNode == root->m_IntegralNode)//mark to edit
					root->m_Dad->m_IntegralWinNode++;
				root->m_IntegralMarked = true;
			}
		}

		BombDis = -1;

		if (root->m_Dad)
		{
			root->m_Dad->m_ChildrnWinrateSum -= (long double)root->m_CountWin / (long double)root->m_CountVisit;
			root->m_Dad->m_ChildrnWinrateSum_Squ -= pow((long double)root->m_CountWin / (long double)root->m_CountVisit, 2);
		}

		if (IsRefreshNodeWinCount(CurrentStatusPlayerId, RandomResult))
			root->m_CountWin++;
		//else
		//	root->m_CountWin--;

		if (root->m_Dad)
		{
			root->m_Dad->m_ChildrnWinrateSum += (long double)root->m_CountWin / (long double)root->m_CountVisit;
			root->m_Dad->m_ChildrnWinrateSum_Squ += pow((long double)root->m_CountWin / (long double)root->m_CountVisit, 2);
		}
		/*if (CurrentStatusPlayerId == 1)
		{
		if (RandomResult == 0)
		{
		root->m_CountWin++;
		}
		}
		else
		{
		if (RandomResult > 0)
		{
		root->m_CountWin++;
		}
		}*/
		//if (!root->m_Children.empty())
		//{
		//	root->m_BestNext = GetBestWay(*root);
		//}
		return 0;
	}

};

class CMultiThreadSimulation :public CSimulation
{
public:
	CNode* UCTSelection(CNode* root) override
	{
		CNode* Chosen{ nullptr };
		double MaxUct{ -1000 };
		if (root == m_Root)
		{
			int thread_num{ m_MaxThreadNum };
			if ((unsigned int)m_MaxThreadNum > (unsigned int)root->m_Children.size())
				thread_num = (unsigned int)root->m_Children.size();
			unsigned int range_len{ (unsigned int)root->m_Children.size() / thread_num };

			for (unsigned int i{ range_len*m_ThreadNum };
				(i < ((m_MaxThreadNum - 1) == m_ThreadNum ? root->m_Children.size() : (m_ThreadNum + 1)*range_len)) && (i < root->m_Children.size());
				i++)
			{
				double val{};
				if (root->m_Children[i]->m_CountVisit > 0)
				{
					val = UctSelectionCalcWeight(root->m_Children[i], root);
				}
				else
				{
					val = 10000 + rand() % 1000;
				}
				if (val > MaxUct)
				{
					MaxUct = val;
					Chosen = (root->m_Children[i]);
				}
			}
		}
		else
		{
			return CSimulation::UCTSelection(root);
		}
		return Chosen;
	}

	void SetMaxThreadNum(int num)
	{
		m_MaxThreadNum = num;
	}

	void SetCurrentThreadNum(int num)
	{
		m_ThreadNum = num;
	}

private:

	int m_MaxThreadNum;
	int m_ThreadNum;
};

class CMonteCarloTree
{
public:
	CMonteCarloTree()
		:m_pRootNode(nullptr)
		, m_simulation(nullptr)
		, m_KeepTime(0)
		, m_MaxSimulationCount(0)
	{

	}

	void Search(CPutCard& BestDest, bool save_root_node)
	{
		if (!save_root_node)
		{
			if (m_pRootNode)
				delete m_pRootNode;
		}
		if (!m_pRootNode)
			m_pRootNode = new CNode();
		m_simulation->Initialize(m_RootStatus, m_pRootNode);
		
		int start_time{ clock() };
		while (
			((m_MaxSimulationCount == 0) || (m_simulation->m_SimuationCount < m_MaxSimulationCount)) &&
			((m_KeepTime == 0) || (clock() - start_time < m_KeepTime))
			)
		{
			if (-1 == m_simulation->Simulation(m_pRootNode)) break;
			m_simulation->SetStatus(m_RootStatus);
		}
		if (!m_pRootNode->m_Children.empty())
		{
			m_pRootNode->m_BestNext = m_simulation->GetBestWay(*m_pRootNode);
		}

		/*cout << "  [simulation_win]\t" << m_simulation->m_WinCount << endl;
		cout << "  [simulation_cou]\t" << m_simulation->m_SimuationCount << endl;
		cout << "  [root_win]\t" << m_pRootNode->m_CountWin << endl;
		cout << "  [root_vis]\t" << m_pRootNode->m_CountVisit << endl;
		cout << "  [win_rate]\t" << m_simulation->WinRateCalc(m_pRootNode->m_BestNext) << endl;

		for (auto i = m_pRootNode->m_Children.begin(); i != m_pRootNode->m_Children.end(); ++i)
		{
			vector<int> out;
			(*i)->m_Action.m_cards.CombineCards(out);
			for (auto i2 = out.begin(); i2 != out.end(); ++i2)
				cout << (*i2) << " ";
			cout << "\t\t"
				<< (double)((double)(*i)->m_CountWin / (double)(*i)->m_CountVisit) << "\t"
				<< (*i)->m_CountWin << "\t"
				<< (*i)->m_CountVisit << "\t"
				<< (*i)->m_ChildrenDepth << endl;
		}*/

		BestDest = m_pRootNode->m_BestNext->m_Action;
		if (!save_root_node)
		{
			delete m_pRootNode;
			m_pRootNode = nullptr;
		}
	}

	void InitializeRootStatus(CTableStatus status)
	{
		m_RootStatus = status;
	}

	template<typename _Ts>
	_Ts* InitializeSimulationObject()
	{
		static_assert(std::is_base_of<CSimulation, _Ts>::value, "Use the type based on CSimulation");
		if (m_simulation)
			delete m_simulation;
		m_simulation = new _Ts;
		return (_Ts*)m_simulation;
	}

	void InitializeControlFactor(int keep_time, int max_count)
	{
		m_KeepTime = keep_time;
		m_MaxSimulationCount = max_count;
	}

	/*void ChangeNodeToNext(CNode* next)
	{
	CNode* newRoot{};
	auto rf = find(m_pRootNode->m_Children.begin(), m_pRootNode->m_Children.end(), next);
	if (rf == m_pRootNode->m_Children.end())
	return;

	newRoot = *rf;
	m_pRootNode->m_Children.erase(rf);
	m_RootStatus.PutCard(newRoot->m_Action);
	delete m_pRootNode;
	m_pRootNode = newRoot;
	}*/

	void ChangeNodeToNext(CPutCard pt)
	{
		CNode* newRoot{};
		for (auto it = m_pRootNode->m_Children.begin(); it != m_pRootNode->m_Children.end(); ++it)
		{
			if ((*it)->m_Action == pt)
			{
				newRoot = *it;
				m_pRootNode->m_Children.erase(it);
				break;
			}
		}
		if (!newRoot)
		{
			CNode* new_node = new CNode;
			new_node->m_Action = pt;
		}
		m_RootStatus.PutCard(newRoot->m_Action);
		delete m_pRootNode;
		newRoot->m_Dad = 0;
		m_pRootNode = newRoot;
	}

private:
	CSimulation* m_simulation;
	CNode* m_pRootNode;
	CTableStatus m_RootStatus;
	int m_KeepTime;
	int m_MaxSimulationCount;
};

class CMultiThreadMonteCarloTree
{
public:
	CMultiThreadMonteCarloTree()
		:m_ThreadId(0)
		, m_KeepSimulation(false)
		, m_pRootNode(nullptr)
		//, m_simulation(nullptr)
		, m_KeepTime(0)
		, m_MaxSimulationCount(0)
	{
		m_ThreadId = 0;
	}

	void ChangeNodeToNext(CNode* next)
	{
		CNode* newRoot{};
		auto rf = find(m_pRootNode->m_Children.begin(), m_pRootNode->m_Children.end(), next);
		if (rf == m_pRootNode->m_Children.end())
			return;

		newRoot = *rf;
		m_pRootNode->m_Children.erase(rf);
		m_RootStatus.PutCard(newRoot->m_Action);
		delete m_pRootNode;
		newRoot->m_Dad = 0;
		m_pRootNode = newRoot;
	}

	CNode* ChangeNodeToNext(CPutCard pt)
	{
		CNode* newRoot{};
		for (auto it = m_pRootNode->m_Children.begin(); it != m_pRootNode->m_Children.end(); ++it)
		{
			if ((*it)->m_Action == pt)
			{
				newRoot = *it;
				m_pRootNode->m_Children.erase(it);
				break;
			}
		}
		if (newRoot == 0)
		{
			CNode* new_node = new CNode;
			new_node->m_Action = pt;
		}
		//m_RootStatus.PutCard(newRoot->m_Action);
		delete m_pRootNode;
		if (newRoot)
			newRoot->m_Dad = 0;
		m_pRootNode = newRoot;

		return m_pRootNode;
	}

	void BeginSearchThread()
	{
		if (m_ThreadId != 0)
			return;
		m_KeepSimulation = true;
		m_ThreadId = (unsigned __int64)CreateThread(0, 0, SearchThreadProc, this, 0, 0);
		return;
	}

	CNode* EndSearchThread(stringstream* oss)
	{
		if (m_MaxSimulationCount == 0 && m_KeepTime == 0)
			m_KeepSimulation = false;
		if (!m_ThreadId)
		{
			return 0;
		}
		WaitForSingleObject((HANDLE)m_ThreadId, INFINITE);
		CloseHandle((HANDLE)m_ThreadId);
		m_ThreadId = 0;

		if (oss)
		{
			*oss << "  [simulation_win]\t" << m_simulations[0]->m_WinCount << endl;
			*oss << "  [simulation_cou]\t" << m_simulations[0]->m_SimuationCount << endl;
			*oss << "  [root_win]\t" << m_pRootNode->m_CountWin << endl;
			*oss << "  [root_vis]\t" << m_pRootNode->m_CountVisit << endl;
			*oss << "  [win_rate]\t" << m_simulations[0]->WinRateCalc(m_pRootNode->m_BestNext) << endl;

			for (auto i = m_pRootNode->m_Children.begin(); i != m_pRootNode->m_Children.end(); ++i)
			{
				vector<int> out;
				(*i)->m_Action.m_cards.CombineCards(out);
				for (auto i2 = out.begin(); i2 != out.end(); ++i2)
					*oss << (*i2) << " ";
				*oss << "\t\t"
					<< (double)((double)(*i)->m_CountWin / (double)(*i)->m_CountVisit) << "\t"
					<< (*i)->m_BasicScore << "\t"
					<< (*i)->m_CountWin << "\t"
					<< (*i)->m_CountVisit << "\t"
					<< (*i)->m_ChildrenDepth << endl;
			}

			*oss << "Put:\n\tplayer[" << m_RootStatus.m_CurrentPlayer << "]:";
			vector<int> put_out;
			m_pRootNode->m_BestNext->m_Action.m_cards.CombineCards(put_out);
			if (put_out.empty())
				*oss << "pass";
			for (auto i = put_out.begin(); i != put_out.end(); ++i)
				*oss << CCardDivider::Num2CardName(*i) << ",";
			*oss << endl;

			*oss << "Status:" << endl;
			for (int i = 0; i < 3; i++)
			{
				*oss << "Player[" << i << "]:";
				for (auto it = m_RootStatus.m_player[i].m_Cards.begin(); it != m_RootStatus.m_player[i].m_Cards.end(); ++it)
				{
					*oss << CCardDivider::Num2CardName(*it) << ',';
				}
				*oss << endl;
			}
		}
		return m_pRootNode->m_BestNext;
	}

	void InitializeRootNode()
	{
		if (m_pRootNode)
			delete m_pRootNode;
		m_pRootNode = new CNode;
	}

	static unsigned long __stdcall SearchThreadProc(void* pthis)
	{
		CMultiThreadMonteCarloTree* pThis = static_cast<CMultiThreadMonteCarloTree*>(pthis);

		//create next nodes
		pThis->m_simulations[0]->Initialize(pThis->m_RootStatus, pThis->m_pRootNode);
		pThis->m_simulations[0]->SetMaxThreadNum(pThis->m_MaxSubThreadNum);
		pThis->m_simulations[0]->SetCurrentThreadNum(0);
		pThis->m_simulations[0]->Simulation(pThis->m_pRootNode);
		pThis->m_simulations[0]->Simulation(pThis->m_pRootNode);
		pThis->m_simulations[0]->SetStatus(pThis->m_RootStatus);

		unsigned int current_thread_num{ (unsigned int)pThis->m_MaxSubThreadNum };
		if (current_thread_num > pThis->m_pRootNode->m_Children.size())
			current_thread_num = (unsigned int)pThis->m_pRootNode->m_Children.size();

		for (unsigned int i{ 1 }; i < current_thread_num; ++i)
		{
			pThis->m_simulations[i]->Initialize(pThis->m_RootStatus, pThis->m_pRootNode);
			pThis->m_simulations[i]->SetMaxThreadNum(pThis->m_MaxSubThreadNum);
			pThis->m_simulations[i]->SetCurrentThreadNum(i);
		}
		if (current_thread_num < 1)
		{
			//there is no possibilities to calc
			return 0;
		}
		unsigned __int64* handles = new unsigned __int64[current_thread_num - 1];

		for (unsigned int i{ 0 }; i < current_thread_num - 1; ++i)
		{
			CSubThreadArgu* arg = new CSubThreadArgu;
			arg->m_pThis = pThis;
			arg->m_ThreadNum = i + 1;
			*(handles + i) = (unsigned __int64)CreateThread(0, 0, SubThreadProc, arg, 0, 0);
		}

		int start_time{ clock() };
		while (
			pThis->m_KeepSimulation
			)
		{
			bool allow_run = ((pThis->m_MaxSimulationCount == 0) || (pThis->m_simulations[0]->m_SimuationCount < pThis->m_MaxSimulationCount)) &&
				((pThis->m_KeepTime == 0) || (clock() - start_time < pThis->m_KeepTime));
			if (!allow_run)
			{
				pThis->m_KeepSimulation = false;
				break;
			}
			pThis->m_simulations[0]->Simulation(pThis->m_pRootNode);
			pThis->m_simulations[0]->SetStatus(pThis->m_RootStatus);

		}

		if (handles)
			WaitForMultipleObjects(current_thread_num - 1, (void**)handles, TRUE, INFINITE);

		if (!pThis->m_pRootNode->m_Children.empty())
		{
			pThis->m_pRootNode->m_BestNext = pThis->m_simulations[0]->GetBestWay(*pThis->m_pRootNode);
		}

		if (pThis->m_TimeLose)
		{
			pThis->m_MaxSimulationCount = 0;
			pThis->m_KeepTime = 0;
		}

		if (handles)
			delete[] handles;
		return 0;
	}

	class CSubThreadArgu
	{
	public:
		int m_ThreadNum;
		void* m_pThis;
	};
	static unsigned long __stdcall SubThreadProc(void* pthis)
	{
		if (!pthis)return 0;
		CSubThreadArgu* arg = (CSubThreadArgu*)pthis;
		CMultiThreadMonteCarloTree* pThis = static_cast<CMultiThreadMonteCarloTree*>(arg->m_pThis);
		int thread_id = arg->m_ThreadNum;
		delete arg;
		while (
			pThis->m_KeepSimulation
			)
		{
			pThis->m_simulations[thread_id]->Simulation(pThis->m_pRootNode);
			pThis->m_simulations[thread_id]->SetStatus(pThis->m_RootStatus);
		}

		return 0;
	}

	void InitializeRootStatus(CTableStatus status)
	{
		m_RootStatus = status;
	}

	template<typename _Ts>
	int InitializeSimulationObject()
	{
		static_assert(std::is_base_of<CMultiThreadSimulation, _Ts>::value, "Use the type based on CSimulation");
		if (m_MaxSubThreadNum < 1)
			return 0;
		for (auto i{ m_simulations.begin() }; i != m_simulations.end(); ++i)
		{
			if (*i)delete *i;
		}
		//if (m_simulation)
		//	delete m_simulation;
		for (int i{}; i < m_MaxSubThreadNum; ++i)
		{
			m_simulations.push_back(new _Ts);
		}
		//m_simulation = new _Ts[m_MaxSubThreadNum];
		return 1;
	}

	void SetTimeLimit(int keep_time, int max_count, bool auto_lose)
	{
		m_KeepTime = keep_time;
		m_MaxSimulationCount = max_count;
		m_TimeLose = auto_lose;
	}

	void SetMaxSubThreadNum(int num)
	{
		m_MaxSubThreadNum = num;
	}

private:
	unsigned __int64 m_ThreadId;
	bool m_KeepSimulation;

	//CMultiThreadSimulation* m_simulation;
	vector<CMultiThreadSimulation*> m_simulations;
	CNode* m_pRootNode;
	CTableStatus m_RootStatus;
	int m_KeepTime;
	int m_MaxSimulationCount;
	bool m_TimeLose;
	int m_MaxSubThreadNum;
};

class CRandomGame :public CMultiThreadSimulation
{
private:
	CCardDivider divider;
public:

	int RandomGame(CTableStatus status) override
	{
		vector<CCardGroup*> all_group;
		CPutCard put;
		//last bomb distance
		int r_BombDis{ -1 };
		int last_not_pass_player_index{ -1 };
		for (; -1 == status.JudgeWinner();)
		{
			//prepare filter argument
			if (r_BombDis != -1)
			{
				if (++r_BombDis > 6)
					r_BombDis = -1;
			}
			last_not_pass_player_index = -1;
			if (status.LastNotPassPlayer())
				last_not_pass_player_index = static_cast<int>(status.LastNotPassPlayer() - status.m_player);
			if (last_not_pass_player_index >= 0)
			{
				if (status.LastNotPassPlayer()->m_LastPut.m_cards.GetEachRootLong() == 4)
					r_BombDis = 1;
			}
			//filter
			if (LockWinner != -1 && r_BombDis > 3 &&
				status.m_CurrentPlayer == LockWinner &&
				PLAYER_OFFSET(status.m_CurrentPlayer, -r_BombDis) != LockEnemy &&
				PLAYER_OFFSET(status.m_CurrentPlayer, -r_BombDis) != LockWinner)
			{
				put.m_IsPass = true;
				status.PutCard(put);
				continue;
			}

			all_group.clear();
			divider.Divide(
				all_group,
				status.m_player[status.m_CurrentPlayer].m_Cards,
				status.LastNotPassPlayer() ? status.LastNotPassPlayer()->m_LastPut.m_cards : status.LastPlayer()->m_LastPut.m_cards,
				status.LastNotPassPlayer()
			);

			if (LockWinner != -1 && last_not_pass_player_index == LockEnemy && status.m_CurrentPlayer != LockWinner)
			{
				for (auto itr{ all_group.begin() }; itr != all_group.end(); ++itr)
				{
					if ((*itr)->GetEachRootLong() == 4)
					{
						delete *itr;
						all_group.erase(itr);
						break;
					}
				}
			}

			if (all_group.size() == 0)
			{
				put.m_IsPass = true;
			}
			else
			{
				put.m_IsPass = false;
				int Choose{ (int)RND(0, all_group.size()) };
				if ((unsigned int)Choose >= all_group.size())
					put.m_IsPass = true;
				else
					put.m_cards = *all_group[Choose];
			}

			status.PutCard(put);

			for (auto it = all_group.begin(); it != all_group.end(); ++it)
				delete *it;
		}

		return status.JudgeWinner();
	}

	int EnumPossibilities(CNode* node, CTableStatus current_status) override
	{
		//for filter
		if (!(LockWinner != -1 && BombDis > 3 &&
			current_status.m_CurrentPlayer == LockWinner &&
			PLAYER_OFFSET(current_status.m_CurrentPlayer, -BombDis) != LockEnemy &&
			PLAYER_OFFSET(current_status.m_CurrentPlayer, -BombDis) != LockWinner))
		{
			vector<CCardGroup*> all_group;
			all_group.clear();

			divider.Divide(
				all_group,
				current_status.m_player[current_status.m_CurrentPlayer].m_Cards,
				current_status.LastNotPassPlayer() ? current_status.LastNotPassPlayer()->m_LastPut.m_cards : current_status.LastPlayer()->m_LastPut.m_cards,
				current_status.LastNotPassPlayer()
			);

			CPlayer judpl;
			for (auto it = all_group.begin(); it != all_group.end(); ++it)
			{
				//filter
				if (LockWinner != -1 && (*it)->GetEachRootLong() == 4)
				{
					int last_not_pass_player_index = static_cast<int>(current_status.LastNotPassPlayer() - current_status.m_player);
					if (last_not_pass_player_index == LockEnemy && current_status.m_CurrentPlayer != LockWinner)
					{
						delete *it;
						continue;
					}
				}

				

				CNode* new_node = new CNode;
				CPutCard new_action;
				new_action.m_IsPass = false;
				new_action.m_cards = **it;
				new_node->m_Action = new_action;
				new_node->m_Dad = node;
				new_node->m_IntegralMarked = false;
				new_node->m_IntegralNode = 0;
				new_node->m_IntegralWinNode = 0;

				if (current_status.m_CurrentPlayer == LockWinner || current_status.m_CurrentPlayer == LockEnemy)
				{
					judpl = current_status.m_player[current_status.m_CurrentPlayer];
					judpl.PutCards(new_action);
					new_node->m_BasicScore = m_Judge.GetCardsPoint(judpl.m_Cards);
				}

				node->m_Children.push_back(new_node);
				delete *it;
			}
			//cout << endl;
		}

		if (current_status.LastNotPassPlayer())
		{
			CNode* pass_node = new CNode;
			CPutCard pass;
			pass.m_IsPass = true;
			pass_node->m_Action = pass;
			pass_node->m_Dad = node;
			pass_node->m_IntegralMarked = false;
			pass_node->m_IntegralNode = 0;
			pass_node->m_IntegralWinNode = 0;

			//if (current_status.m_CurrentPlayer == LockWinner || current_status.m_CurrentPlayer == LockEnemy)
			//	pass_node->m_BasicScore = m_Judge.GetCardsPoint(current_status.m_player[current_status.m_CurrentPlayer].m_Cards);
			pass_node->m_BasicScore = 0;

			node->m_Children.push_back(pass_node);
		}

		return 0;
	}

	inline double UctSelectionCalcWeight(CNode* node, CNode* dad_node) override
	{
		//double win_rate{ (double)node->m_CountWin / (double)node->m_CountVisit };
		double win_rate{ WinRateCalc(node) };
		//double uct = win_rate + Explore * sqrt(2 * log(dad_node->m_CountVisit) / (double)(node->m_CountVisit));
		double risk{};
		if (node->m_Children.size())
		{
			long double risk_base
			{ 
				(node->m_ChildrnWinrateSum_Squ / (double)node->m_Children.size() - pow((node->m_ChildrnWinrateSum / (double)node->m_Children.size()), 2)) 
			};
			if (risk_base > 0)
			{
				risk = sqrt(risk_base*(double)node->m_Children.size());
			}
		}
		double uct = win_rate + Explore * sqrt(2 * log(dad_node->m_CountVisit) / (double)(node->m_CountVisit));
		if (risk)uct -= AvoidRisk*risk;
		return uct;
	}

	inline void ConvergenceUCTScopeFactor(double decay_factor) override
	{
		Explore /= decay_factor;
	}

	static void ConvergenceUCTScopeFactor_Static(double decay_factor)
	{
		Explore /= decay_factor;
	}

	bool IsRefreshNodeWinCount(int current_id, int game_result) override
	{
		if (PLAYER_OFFSET(current_id, -1) == LockEnemy)
		{
			if (game_result == LockEnemy)
				return true;
		}
		else
		{
			if (LockWinner != -1)
			{
				if (game_result == LockWinner)
					return true;
			}
			else
			{
				if (game_result != LockEnemy)
					return true;
			}
		}
		return false;
	}

	inline double WinRateCalc(CNode* node) override
	{
		if (!node)
			return 0;
		double wr{ (double)node->m_CountWin / (double)node->m_CountVisit };
		wr += node->m_BasicScore*0.1;
		if (node->m_Action.m_cards.GetEachRootLong() == 4)
		{
			wr *= BenefitGain;
		}
		return wr;
	}

	static double Explore;

	static int LockEnemy;

	static int LockWinner;

	static double BenefitGain;

	static double AvoidRisk;
};

double CRandomGame::Explore = 0;

int CRandomGame::LockEnemy = 0;

int CRandomGame::LockWinner = -1;

double CRandomGame::BenefitGain = 0;

double CRandomGame::AvoidRisk = 0;