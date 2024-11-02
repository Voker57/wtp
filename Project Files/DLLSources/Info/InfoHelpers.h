#pragma once

template<class T>
class InfoHelperVector
{
public:
	const std::vector<T>& getVector() const;

	bool read(CvXMLLoadUtility* pXML, const char* szType, const char* szTagName);
private:
	std::vector<T> m_vector;
};

template<class T>
const std::vector<T>& InfoHelperVector<T>::getVector() const
{
	return m_vector;
}

template<class T>
bool InfoHelperVector<T>::read(CvXMLLoadUtility* pXML, const char* szType, const char* szTagName)
{
	CvDLLXmlIFaceBase* pIFace = gDLL->getXMLIFace();
	FXml* XML = pXML->GetXML();

	if (pIFace->SetToChildByTagName(pXML->GetXML(), szTagName))
	{
		unsigned int iLength = 0;

		if (pIFace->SetToChild(XML))
		{
			do
			{
				T buffer;
				m_vector.push_back(buffer);
				m_vector[iLength].read(pXML, szType);
				++iLength;
			} while (pIFace->NextSibling(XML));

			pIFace->SetToParent(XML);
		}

		pIFace->SetToParent(XML);
	}
	return true;
}
