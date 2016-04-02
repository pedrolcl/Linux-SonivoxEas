#include <QCoreApplication>
#include <signal.h>
#include "synthesizer.h"

static QTextStream cerr(stderr, QIODevice::WriteOnly);
static QProcessMIDI* dmp = 0;

void signalHandler(int sig)
{
    if (sig == SIGINT)
        cerr << "Caught a SIGINT. Exiting" << endl;
    else if (sig == SIGTERM)
        cerr << "Caught a SIGTERM. Exiting" << endl;
    if (dmp != 0) {
        dmp->stop();
    }
}

int main(int argc, char **argv)
{
    const QString errorstr = "Fatal error from the ALSA sequencer. "
        "This usually happens when the kernel doesn't have ALSA support, "
        "or the device node (/dev/snd/seq) doesn't exists, "
        "or the kernel module (snd_seq) is not loaded. "
        "Please check your ALSA/MIDI configuration.";
    QCoreApplication app(argc, argv);

    try {
        dmp = new QProcessMIDI();
        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);
        dmp->run();
    } catch (const SequencerError& ex) {
        cerr << errorstr << " Returned error was: " << ex.qstrError() << endl;
    } catch (...) {
        cerr << errorstr << endl;
    }
    delete dmp;
    return 0;
}
