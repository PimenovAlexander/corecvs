#include <fstream>

#include "core/reflection/commandLineSetter.h"
#include "core/utils/preciseTimer.h"
#include "core/buffers/bufferFactory.h"


#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/GetObjectRequest.h>

#include <pqxx/pqxx>

#ifdef WITH_LIBPNG
#include "libpngFileReader.h"
#endif

using namespace std;
using namespace corecvs;


void testAWSBucketAccess( CommandLineSetter &s )
{
    Aws::SDKOptions options;
    Aws::InitAPI(options);
    SYNC_PRINT(("Aws::InitAPI(): processed\n"));
    {
        // Assign these values before running the program
        const Aws::String bucket_name = s.getString("bucket", "apimenov-bucket").c_str();
        const Aws::String object_name = s.getString("file", "test.txt").c_str();  // For demo, set to a text file

        SYNC_PRINT(("Will request bucket <%s>\n", bucket_name.c_str()));
        SYNC_PRINT(("Will request file   <%s>\n", object_name.c_str()));


        // Set up the request
        Aws::Client::ClientConfiguration clientConfiguration;
        clientConfiguration.region = Aws::Region::EU_NORTH_1;
        Aws::S3::S3Client s3_client(clientConfiguration);

        Aws::S3::Model::GetObjectRequest object_request;
        object_request.SetBucket(bucket_name);
        object_request.SetKey(object_name);

        SYNC_PRINT(("Forming AWS request\n"));


        // Get the object
        auto get_object_outcome = s3_client.GetObject(object_request);
        if (get_object_outcome.IsSuccess())
        {
            // Get an Aws::IOStream reference to the retrieved file
            auto &retrieved_file = get_object_outcome.GetResultWithOwnership().GetBody();

#if 1
            // Output the first line of the retrieved text file
            std::cout << "Beginning of file contents:\n";
            char file_data[255] = { 0 };
            retrieved_file.getline(file_data, 254);
            std::cout << file_data << std::endl;
#else
            // Alternatively, read the object's contents and write to a file
            const char * filename = "/PATH/FILE_NAME";
            std::ofstream output_file(filename, std::ios::binary);
            output_file << retrieved_file.rdbuf();
#endif
        }
        else
        {
            auto error = get_object_outcome.GetError();
            std::cout << "ERROR: " << error.GetExceptionName() << ": "
                << error.GetMessage() << std::endl;
        }
    }
    Aws::ShutdownAPI(options);
}

void testPostgresAccess(  CommandLineSetter &s  )
{
    std::string db_url = s.getString("url", "postgresql://");

    SYNC_PRINT(("testPostgresAccess: opening <%s>\n", db_url.c_str()));
    pqxx::connection c(db_url);
    pqxx::work txn(c);

    pqxx::result result{txn.exec("SELECT * FROM input_videos")};
    int rowcount = 0;
    for (pqxx::row row: result) {
        cout << "ROW: ";
        for (int i = 0; i < (int)row.size(); i++)
        {
            cout << row[i].c_str() << " ";
        }
        cout << endl;

        rowcount++;
    }
}

int main (int argC, char **argV)
{
    CommandLineSetter s(argC, argV);

#ifdef WITH_LIBPNG
    LibpngFileReader::registerMyself();
    LibpngRuntimeTypeBufferLoader::registerMyself();
    LibpngFileSaver::registerMyself();
    SYNC_PRINT(("Libpng support on\n"));
#endif

    if (s.hasOption("postgres"))
    {
        testPostgresAccess(s);
        return 0;
    }

    if (s.hasOption("aws"))
    {
        testAWSBucketAccess(s);
        return 0;
    }

    return 0;
}

