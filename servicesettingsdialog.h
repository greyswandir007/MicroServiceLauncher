#ifndef SERVICESETTINGSDIALOG_H
#define SERVICESETTINGSDIALOG_H

#include "serviceconfigurer.h"
#include "serviceparams.h"

#include <QDialog>

namespace Ui {
class ServiceSettingsDialog;
}

class ServiceSettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit ServiceSettingsDialog(ServiceConfigurer *serviceConfigurer, QWidget *parent = nullptr);
    ~ServiceSettingsDialog();

    void setServiceParams(ServiceParams serviceParams);
    ServiceParams getServiceParams();

public slots:
    void paramChanged();

private:
    void updateServiceParams();

    Ui::ServiceSettingsDialog *ui;
    ServiceParams serviceParams;
    ServiceConfigurer *serviceConfigurer;
};

#endif // SERVICESETTINGSDIALOG_H
